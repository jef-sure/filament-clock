#include "spi_shiftout.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <stdlib.h>
#include <string.h>

static const char *TAG = "spi_shiftout";

#define OE_RESOLUTION LEDC_TIMER_8_BIT
#define OE_MAX_VALUE  ((1U << OE_RESOLUTION))

spi_device_handle_t spi_shiftout_spi_init(int32_t frequency, gpio_num_t data_pin, gpio_num_t clock_pin,
                                          spi_host_device_t host)
{
    // Configure the SPI bus
    spi_bus_config_t buscfg = {
        .miso_io_num     = -1,        //
        .mosi_io_num     = data_pin,  //
        .sclk_io_num     = clock_pin, //
        .quadwp_io_num   = -1,        //
        .quadhd_io_num   = -1,        //
        .max_transfer_sz = 0,         //
        .flags           = 0          //
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = frequency, // Clock out
        .mode           = 0,         // SPI mode 0
        .spics_io_num   = -1,        // CS pin
        .queue_size     = 1,         //
        .pre_cb         = 0,         //
    };
    esp_err_t rc;
    // Initialize the SPI bus
    rc = spi_bus_initialize(host, &buscfg, SPI_DMA_CH_AUTO);
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "Failed spi_bus_initialize");
        return 0;
    }
    spi_device_handle_t spi;
    rc = spi_bus_add_device(host, &devcfg, &spi);
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "Failed spi_bus_add_device");
        return 0;
    }
    return spi;
}

spi_shiftout_t *spi_shiftout_init(spi_device_handle_t spi, gpio_num_t latch_pin, gpio_num_t output_enable_pin,
                                  ledc_channel_t pwm_channel, uint8_t shift_out_length)
{
    spi_shiftout_t *cfg = (spi_shiftout_t *)calloc(1, sizeof(spi_shiftout_t) + shift_out_length - 1);
    if (!cfg) {
        ESP_LOGE(TAG, "Failed to allocate memory for spi_shiftout_t");
        return NULL;
    }
    cfg->spi                    = spi;
    cfg->latch_pin              = latch_pin;
    cfg->output_enable_pin      = output_enable_pin;
    cfg->output_enable_value    = 0; // No output enable by default
    cfg->ledc_duty              = OE_MAX_VALUE - cfg->output_enable_value;
    cfg->shift_out_length       = shift_out_length;
    cfg->ledc_channel           = pwm_channel;
    ledc_timer_t ledc_timer_num = LEDC_TIMER_0;
    if (output_enable_pin >= 0) {
        // Configure OE pin for PWM (LEDC)
        ledc_timer_config_t ledc_timer = {
            .speed_mode      = LEDC_LOW_SPEED_MODE, //
            .timer_num       = ledc_timer_num,      //
            .duty_resolution = OE_RESOLUTION,       //
            .freq_hz         = 4000,                // TODO: test this value
            .clk_cfg         = LEDC_AUTO_CLK        //
        };
        ledc_timer_config(&ledc_timer);
        ledc_channel_config_t ledc_channel = {
            .gpio_num            = cfg->output_enable_pin, //
            .speed_mode          = LEDC_LOW_SPEED_MODE,    //
            .channel             = cfg->ledc_channel,      //
            .timer_sel           = ledc_timer_num,         //
            .duty                = cfg->ledc_duty,         //
            .hpoint              = 0,                      //
            .flags.output_invert = 0                       //
        };
        ledc_channel_config(&ledc_channel);
    }
    gpio_config_t latch_conf = {
        .pin_bit_mask = (1ULL << latch_pin),   //
        .mode         = GPIO_MODE_OUTPUT,      //
        .pull_up_en   = GPIO_PULLUP_ENABLE,    //
        .pull_down_en = GPIO_PULLDOWN_DISABLE, //
        .intr_type    = GPIO_INTR_DISABLE      //
    };
    gpio_config(&latch_conf);
    gpio_set_level(latch_pin, 1);
    return cfg;
}

void spi_shiftout_write(spi_shiftout_t *cfg)
{
    esp_err_t         ret;
    spi_transaction_t t;
    gpio_set_level(cfg->latch_pin, 0);
    memset(&t, 0, sizeof(t));                                // Zero out the transaction
    t.length    = 8 * cfg->shift_out_length;                 // Command is 8 bits
    t.tx_buffer = cfg->shift_out;                            // Data
    ret         = spi_device_polling_transmit(cfg->spi, &t); // Transmit!
    gpio_set_level(cfg->latch_pin, 1);
    assert(ret == ESP_OK); // Should have had no issues.
    if (cfg->output_enable_pin >= 0 && cfg->ledc_duty != OE_MAX_VALUE - cfg->output_enable_value) {
        cfg->ledc_duty = OE_MAX_VALUE - cfg->output_enable_value;
        ledc_set_duty(LEDC_LOW_SPEED_MODE, cfg->ledc_channel, cfg->ledc_duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, cfg->ledc_channel);
    }
}
