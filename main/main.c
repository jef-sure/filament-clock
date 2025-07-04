/*
 * Filament Clock
 * Copyright Anton Petrusevich 2025
 * License: MIT
 */

#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "esp_err.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <freertos/semphr.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "clock_hands.h"
#include "font2x7.h"
#include "font3x5.h"
#include "font3x7.h"
#include "http_server.h"
#include "led_matrix.h"
#include "nvs_config.h"
#include "provisioning.h"
#include "spi_shiftout.h"
#include "weather.h"

enum
{
    // 0 and 1 are free for use
    // 2 is used for strapping
    CFLightnessChannel = LEDC_CHANNEL_0, // LEDC channel for clock face light
    CFOutputEnablePin  = GPIO_NUM_7,
    CFLatchPin         = GPIO_NUM_10,
    DataPin            = GPIO_NUM_6,
    ClockPin           = GPIO_NUM_4,
    ShiftOutFreq       = 1000000,
    CFShiftOutLength   = 9,
    LMLightnessChannel = LEDC_CHANNEL_1, // LEDC channel for LED matrix light
    LMOutputEnablePin  = GPIO_NUM_9,
    LMLatchPin         = GPIO_NUM_5,
    LMShiftOutLength   = 4,
    SPI_HOST           = SPI2_HOST,
};

enum
{
    DisplayModeWeather,
    DisplayModeWeb,
    DisplayModeClock,
    DisplayModeStarting
} display_mode = DisplayModeStarting;

static const char TAG[]          = "MAIN APP";
SemaphoreHandle_t xSemClockHands = NULL;
clock_hands_t    *clock_hands    = NULL;
SemaphoreHandle_t xSemLedMatrix  = NULL;
led_matrix_t     *led_matrix     = NULL;

typedef struct
{
    int        temperature;
    weather_t *weather;
    bool       is_day; // true if day, false if night
} weather_info_t;

weather_info_t weather_info = {
    .temperature = 0,
    .weather     = NULL,
    .is_day      = true, // Default to day
};

void draw_weather_info(led_matrix_t *matrix, void *data)
{
    weather_info_t *weather_info = (weather_info_t *)data;
    if (matrix == NULL || weather_info == NULL || weather_info->weather == NULL) {
        ESP_LOGE("show_weather_info", "Invalid matrix or weather information");
        return;
    }
    led_matrix_clear(matrix);
    weather_icon_id_t icon_id = weather_info->is_day ? weather_info->weather->dayIconId : weather_info->weather->nightIconId;
    const uint8_t    *icon    = getWeatherIcon16x10(icon_id);
    if (icon == NULL) {
        ESP_LOGE("show_weather_icon", "Icon not found for ID: %d", icon_id);
        return;
    }
    for (int y = 0; y < 10; y++) {
        for (int xb = 0; xb < 2; xb++) {
            uint8_t mask = 0x80;
            for (int x = 0; x < 8; x++, mask >>= 1) {
                uint8_t value = !!(icon[y * 2 + xb] & mask);
                led_matrix_set(matrix, xb * 8 + x, y, value);
            }
        }
    }
    // Show temperature
    bool is_negative   = weather_info->temperature < 0;
    int  temp_abs      = is_negative ? -weather_info->temperature : weather_info->temperature;
    int  tens          = temp_abs / 10;
    int  ones          = temp_abs % 10;
    int  digits        = tens == 0 ? 1 : 2; // If tens is 0, we only show one digit
    int  first_digit_x = 17 - digits * 4;   // Start from the right side
    if (is_negative) {
        led_matrix_set(matrix, first_digit_x - 1, 13, 1);
        led_matrix_set(matrix, first_digit_x - 2, 13, 1);
        led_matrix_set(matrix, first_digit_x - 3, 13, 1);
    }
    if (tens > 0) {
        draw_digit_3x5(matrix, tens, first_digit_x, 10);
        first_digit_x += 4; // Move to the next digit position
    }
    draw_digit_3x5(matrix, ones, first_digit_x, 10);
}

void show_weather_info_sync()
{
    led_matrix_draw_sync(led_matrix, xSemLedMatrix, draw_weather_info, &weather_info);
}

void show_service_info(led_matrix_t *matrix, void *data)
{
    char *service_name = (char *)data; // CLK-1234
    ESP_LOGI(TAG, "Showing service info: %s", service_name);
    led_matrix_clear(matrix);
    draw_digit_3xCLK(matrix); // Show clock icon
    for (int i = 0; i < 4; i++) {
        draw_digit_3x(matrix, service_name[i + 4] - '0', i * 4, 8);
    }
}

void show_ble_id(char *service_name)
{
    led_matrix_draw_sync(led_matrix, xSemLedMatrix, show_service_info, service_name);
}

void show_webserver_ip(led_matrix_t *matrix, void *data)
{
    if (matrix == NULL) {
        ESP_LOGE("show_clock", "Invalid matrix");
        return;
    }
    led_matrix_clear(matrix);
    esp_netif_ip_info_t ip_infoMyIf;
    esp_netif_get_ip_info(esp_netif_get_default_netif(), &ip_infoMyIf);
    union
    {
        uint8_t  bytes[4];
        uint32_t ip;
    } ip_union;
    ip_union.ip = ip_infoMyIf.ip.addr;
    ESP_LOGI(TAG, "IP: %d.%d.%d.%d", ip_union.bytes[0], ip_union.bytes[1], ip_union.bytes[2], ip_union.bytes[3]);
    // Show IP address on the matrix
    for (int i = 2; i < 4; i++) {
        uint8_t y           = (i & 1) * 8; // 0 for first byte, 8 for second byte
        int     byte        = ip_union.bytes[i];
        int     digit_count = 3;
        while (byte) {
            int digit = byte % (uint8_t)10;
            byte /= 10;
            int x = digit_count * 4; // Start from the right side
            draw_digit_3x(matrix, digit, x, y);
            digit_count--;
        }
    }
}

void task_shiftout(void *arg)
{
    int     led_matrix_baccu = 0;
    int64_t next_yeild       = 0;
    while (1) {
        bool lm_was_on = false;
        if (xSemaphoreTake(xSemClockHands, pdMS_TO_TICKS(1)) == pdTRUE) {
            if (clock_hands != NULL && clock_hands->shiftout != NULL && !clock_hands->is_sent) {
                spi_shiftout_write(clock_hands->shiftout);
                clock_hands->is_sent = true;
            }
            xSemaphoreGive(xSemClockHands);
        }
        if (led_matrix != NULL && led_matrix->led_matrix_cfg != NULL) {
            if (xSemaphoreTake(xSemLedMatrix, pdMS_TO_TICKS(1)) == pdTRUE) {
                led_matrix_baccu += led_matrix->brightness;
                for (int j = 0; j < led_matrix->height; j++) {
                    if (led_matrix_baccu >= LMBRIGHTNESS_RESOLUTION) led_matrix_show_row(led_matrix, j);
                    else led_matrix_show_black_row(led_matrix, j);
                }
                if (led_matrix_baccu >= LMBRIGHTNESS_RESOLUTION) {
                    lm_was_on = true;
                    led_matrix_baccu -= LMBRIGHTNESS_RESOLUTION;
                }
                xSemaphoreGive(xSemLedMatrix);
            }
        }
        if (next_yeild < esp_timer_get_time()) {
            if(lm_was_on) {
                led_matrix_show_black_row(led_matrix, led_matrix->height - 1);
            }
            vTaskDelay(1);
            next_yeild = esp_timer_get_time() + 10000; // Yield every 10 ms
        }
    }
}

void app_main(void)
{
    xSemClockHands = xSemaphoreCreateMutex();
    xSemaphoreGive(xSemClockHands);
    xSemLedMatrix = xSemaphoreCreateMutex();
    xSemaphoreGive(xSemLedMatrix);
    // Initialize SPI for shift out
    spi_device_handle_t spi = spi_shiftout_spi_init(ShiftOutFreq, DataPin, ClockPin, SPI_HOST);
    if (spi == NULL) {
        ESP_LOGE("spi_shiftout", "Failed spi_shiftout_spi_init");
        return;
    }
    clock_hands = clock_hands_init(spi, CFLatchPin, CFOutputEnablePin, CFLightnessChannel, CFShiftOutLength);
    if (clock_hands == NULL) {
        ESP_LOGE("app_main", "Failed to initialize clock hands");
        return;
    }
    clock_hands_set(clock_hands, 0, 0);
    spi_shiftout_t *led_matrix_cfg = spi_shiftout_init(spi, LMLatchPin, LMOutputEnablePin, LMLightnessChannel, LMShiftOutLength);
    if (led_matrix_cfg == NULL) {
        ESP_LOGE("app_main", "Failed to initialize led matrix spi");
        return;
    }
    led_matrix = led_matrix_init(led_matrix_cfg, 16, 16);
    if (led_matrix == NULL) {
        ESP_LOGE("app_main", "Failed to initialize led matrix structure");
        return;
    }
    led_matrix->brightness = 1;
    xTaskCreate(task_shiftout, "task_shiftout", 4096, NULL, 24, NULL);
    provisioning_setup(show_ble_id, false);                  // Start provisioning if needed
    str_t *nvs_ip_info_key = get_nvs_str_key("ip_info_key"); //
    str_t *nvs_location    = get_nvs_str_key("location");    //
    if (nvs_ip_info_key == NULL || nvs_location == NULL || str_length(nvs_ip_info_key) == 0 || str_length(nvs_location) == 0) {
        ESP_LOGI(TAG, "No IP info or location in NVS, starting web server");
        if (!start_webserver()) {
            ESP_LOGE(TAG, "Failed to start web server");
        } else {
            ESP_LOGI(TAG, "Web server started successfully");
            led_matrix_draw_sync(led_matrix, xSemLedMatrix, show_webserver_ip, NULL);
        }
    } else {
        ESP_LOGI(TAG, "IP info: %s; Location: %s", str_c(nvs_ip_info_key), str_c(nvs_location));
    }
    esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_init();
    esp_sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);

    int64_t now_us   = esp_timer_get_time();
    int64_t fc_start = now_us;
    while (1) {
        now_us = esp_timer_get_time();
        if (now_us - fc_start > 1000000ll /* 1 second */) {
            float          fps         = 1000000.0f * 1 / (float)(now_us - fc_start);
            bool           sntp_synced = esp_sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED;
            struct timeval now;
            gettimeofday(&now, 0);
            time_t     t      = now.tv_sec;
            struct tm *tm_now = localtime(&t);
            ESP_LOGI(TAG, "FPS: %f; sntp_synced: %d; time: %02d:%02d:%02d", fps, sntp_synced, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
            fc_start = now_us;
        }
        vTaskDelay(1);
    }
}