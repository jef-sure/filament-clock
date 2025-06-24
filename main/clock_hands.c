#include "esp_err.h"
#include "esp_log.h"
#include <string.h>

#include "clock_hands.h"

clock_hands_t *clock_hands_init(spi_device_handle_t spi, gpio_num_t CFLatchPin, gpio_num_t CFOutputEnablePin, ledc_channel_t CFLightnessChannel,
                                uint8_t CFShiftOutLength)
{
    spi_shiftout_t *cfg = spi_shiftout_init(spi, CFLatchPin, CFOutputEnablePin, CFLightnessChannel, CFShiftOutLength);
    if (cfg == NULL) {
        ESP_LOGE("spi_shiftout", "Failed to initialize shift out clock hands");
        return NULL;
    }
    clock_hands_t *hands = calloc(1, sizeof(clock_hands_t));
    if (hands == NULL) {
        ESP_LOGE("clock_hands", "Failed to allocate memory for clock hands");
        free(cfg);
        return NULL;
    }
    hands->shiftout = cfg;
    hands->hours    = 0;
    hands->minutes  = 0;
    hands->is_sent  = false;
    return hands;
}

void clock_hands_set(clock_hands_t *hands, uint8_t hours, uint8_t minutes)
{
    uint8_t h_i = (hours % 12) + 60u;
    uint8_t m_i = minutes % 60u;
    if (hands == NULL || hands->shiftout == NULL) {
        ESP_LOGE("clock_hands", "Invalid clock hands or shift out configuration");
        return;
    }
    hands->hours        = hours;
    hands->minutes      = minutes;
    hands->is_sent      = false;
    spi_shiftout_t *cfg = hands->shiftout;
    memset(cfg->shift_out, 0, cfg->shift_out_length);
    cfg->shift_out[cfg->shift_out_length - h_i / 8 - 1] |= (1 << (h_i % 8));
    cfg->shift_out[cfg->shift_out_length - m_i / 8 - 1] |= (1 << (m_i % 8));
}

void clock_hands_sync_set(clock_hands_t *hands, SemaphoreHandle_t syncMutex, uint8_t hours, uint8_t minutes)
{
    while (xSemaphoreTake(syncMutex, 0) != pdTRUE) {
        // Wait for the semaphore to be available
        vTaskDelay(1);
    }
    clock_hands_set(hands, hours, minutes);
    xSemaphoreGive(syncMutex);
}