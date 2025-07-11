#pragma once

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    gpio_num_t          output_enable_pin;
    uint32_t            output_enable_value;
    uint32_t            ledc_duty;
    gpio_num_t          latch_pin;
    ledc_channel_t      ledc_channel;
    spi_device_handle_t spi;
    uint8_t             shift_out_length;
    uint8_t             shift_out[1] __attribute__((aligned(4))); // 4-byte aligned
} spi_shiftout_t;

spi_device_handle_t spi_shiftout_spi_init(int32_t frequency, gpio_num_t data_pin, gpio_num_t clock_pin, spi_host_device_t host);

spi_shiftout_t *spi_shiftout_init(spi_device_handle_t spi, gpio_num_t latch_pin, gpio_num_t output_enable_pin, ledc_channel_t pwm_channel,
                                  uint8_t shift_out_length);

void spi_shiftout_write(spi_shiftout_t *cfg);

#ifdef __cplusplus
}
#endif