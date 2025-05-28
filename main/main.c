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
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "spi_shiftout.h"

enum
{
    // 0 and 1 are free for use
    // 2 is used for strapping
    OutputEnablePin = GPIO_NUM_0,
    LatchPin        = GPIO_NUM_10,
    DataPin         = GPIO_NUM_6,
    ClockPin        = GPIO_NUM_4,
    ShiftOutFreq    = 2000000,
    ShiftOutLength  = 9,
};

spi_shiftout_t *init_clock_hands(void)
{
    spi_shiftout_t *cfg =
        spi_shiftout_init(ShiftOutFreq, DataPin, ClockPin, LatchPin, OutputEnablePin, SPI2_HOST, ShiftOutLength);
    if (cfg == NULL) {
        ESP_LOGE("spi_shiftout", "Failed to initialize shift out clock hands");
        return NULL;
    }
    return cfg;
}

void clock_set_hands(spi_shiftout_t *cfg, uint8_t hours, uint8_t minutes)
{
    uint8_t h_i = (hours % 12) + 60u;
    uint8_t m_i = minutes % 60u;
    memset(cfg->shift_out, 0, cfg->shift_out_length);
    cfg->shift_out[cfg->shift_out_length - h_i / 8 - 1] |= (1 << (h_i % 8));
    cfg->shift_out[cfg->shift_out_length - m_i / 8 - 1] |= (1 << (m_i % 8));
    spi_shiftout_write(cfg);
}

void app_main(void)
{
    spi_shiftout_t *cfg = init_clock_hands();
    if (cfg == NULL) {
        ESP_LOGE("app_main", "Failed to initialize clock hands");
        return;
    }
    while (1) {
        for (int i = 0; i < 24 * 60; i++) {
            clock_set_hands(cfg, i / 60, i % 60);
            vTaskDelay(pdMS_TO_TICKS(500)); // Delay for 500 ms
            cfg->output_enable_value = 1;
        }
    }
}
