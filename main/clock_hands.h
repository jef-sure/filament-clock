#pragma once
#include "spi_shiftout.h"
#include <freertos/semphr.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    spi_shiftout_t *shiftout;
    uint8_t         hours;
    uint8_t         minutes;
    bool            is_sent;
} clock_hands_t;

clock_hands_t *clock_hands_init(spi_device_handle_t spi, gpio_num_t CFLatchPin, gpio_num_t CFOutputEnablePin, ledc_channel_t CFLightnessChannel,
                                uint8_t CFShiftOutLength);
void           clock_hands_set(clock_hands_t *hands, uint8_t hours, uint8_t minutes);
void           clock_hands_sync_set(clock_hands_t *hands, SemaphoreHandle_t syncMutex, uint8_t hours, uint8_t minutes);

#ifdef __cplusplus
}
#endif