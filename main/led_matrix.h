#pragma once
#include "spi_shiftout.h"
#include <freertos/semphr.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define LMBRIGHTNESS_RESOLUTION 16

typedef struct
{
    spi_shiftout_t *led_matrix_cfg;
    int             brightness;    // Brightness of the LED matrix, 0-255
    uint8_t         width;         // Width of the LED matrix
    uint8_t         height;        // Height of the LED matrix
    uint8_t         led_matrix[1]; // Dynamic array for LED matrix, size determined at runtime
} led_matrix_t;

typedef void (*led_matrix_draw_func_t)(led_matrix_t *matrix, void *data);

led_matrix_t *led_matrix_init(spi_shiftout_t *cfg, uint8_t width, uint8_t height);
void          led_matrix_set(led_matrix_t *matrix, uint8_t x, uint8_t y, uint8_t value);
void          led_matrix_show_row(led_matrix_t *matrix, uint8_t row);
void          led_matrix_show_black_row(led_matrix_t *matrix, uint8_t row);
void          led_matrix_clear(led_matrix_t *matrix);
void          led_matrix_draw_sync(led_matrix_t *matrix, SemaphoreHandle_t syncMutex, led_matrix_draw_func_t drawFunc, void *data);

#ifdef __cplusplus
}
#endif
