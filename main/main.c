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
#include "led_matrix.h"
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
    ShiftOutFreq       = 2000000,
    CFShiftOutLength   = 9,
    LMLightnessChannel = LEDC_CHANNEL_1, // LEDC channel for LED matrix light
    LMOutputEnablePin  = GPIO_NUM_9,
    LMLatchPin         = GPIO_NUM_5,
    LMShiftOutLength   = 4,
    SPI_HOST           = SPI2_HOST,
};

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

void task_shiftout(void *arg)
{
    while (1) {
        if (xSemaphoreTake(xSemClockHands, pdMS_TO_TICKS(1)) == pdTRUE) {
            if (clock_hands != NULL && clock_hands->shiftout != NULL && !clock_hands->is_sent) {
                spi_shiftout_write(clock_hands->shiftout);
                clock_hands->is_sent = true;
            }
            xSemaphoreGive(xSemClockHands);
        }
        if (led_matrix != NULL && led_matrix->led_matrix_cfg != NULL) {
            for (int j = 0; j < led_matrix->height; j++) {
                if (xSemaphoreTake(xSemLedMatrix, pdMS_TO_TICKS(1)) == pdTRUE) {
                    led_matrix_show_row(led_matrix, j);
                    xSemaphoreGive(xSemLedMatrix);
                } else {
                    --j; // If we can't take the semaphore, retry the same row
                }
                vTaskDelay(1);
            }
        }
        vTaskDelay(1);
    }
}

void provisioning_setup(bool force_provisioning);

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
    led_matrix->led_matrix_cfg->output_enable_value = 1; // Enable output
    xTaskCreate(task_shiftout, "task_shiftout", 4096, NULL, 0, NULL);
    provisioning_setup(false); // Start provisioning if needed
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