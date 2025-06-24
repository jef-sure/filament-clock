#include "esp_err.h"
#include "esp_log.h"
#include <string.h>

#include "led_matrix.h"

led_matrix_t *led_matrix_init(spi_shiftout_t *cfg, uint8_t width, uint8_t height)
{
    size_t        size   = sizeof(led_matrix_t) + ((width + 7u) / 8u * height) - 1;
    led_matrix_t *matrix = (led_matrix_t *)calloc(1, size);
    if (!matrix) {
        ESP_LOGE("led_matrix", "Failed to allocate memory for LED matrix");
        return NULL;
    }
    memset(matrix->led_matrix, 0xff, (width + 7u) / 8u * height);
    matrix->led_matrix_cfg = cfg;
    matrix->width          = width;
    matrix->height         = height;
    return matrix;
}

void led_matrix_set(led_matrix_t *matrix, uint8_t x, uint8_t y, uint8_t value)
{
    if (x >= matrix->width || y >= matrix->height) {
        ESP_LOGE("led_matrix", "Coordinates out of bounds: (%d, %d)", x, y);
        return;
    }
    uint32_t row_size  = (matrix->width + 7u) / 8u;
    uint32_t row_index = row_size - x / 8u - 1u; // Calculate the row index based on x
    uint32_t index     = row_size * y + row_index;
    uint32_t bit       = x % 8u;
    if (!value) {
        matrix->led_matrix[index] |= (1 << bit);
    } else {
        matrix->led_matrix[index] &= ~(1 << bit);
    }
}

void led_matrix_show_row(led_matrix_t *matrix, uint8_t row)
{
    if (row >= matrix->height) {
        ESP_LOGE("led_matrix", "Row out of bounds: %d", row);
        return;
    }
    if (matrix == NULL || matrix->led_matrix_cfg == NULL) {
        ESP_LOGE("led_matrix", "LED matrix or configuration is NULL");
        return;
    }
    // Prepare the shift out data for the specified row
    uint32_t row_size       = (matrix->width + 7u) / 8u;
    uint32_t index          = row_size * row;
    uint8_t *shift_out      = matrix->led_matrix_cfg->shift_out;
    uint32_t row_shift_size = matrix->led_matrix_cfg->shift_out_length - row_size;
    memcpy(shift_out, &matrix->led_matrix[index], row_size);
    memset(shift_out + row_size, 0xff, row_shift_size);
    uint32_t bit           = (row % 8u);
    uint32_t row_bit_index = matrix->led_matrix_cfg->shift_out_length - row / 8u - 1u;
    shift_out[row_bit_index] &= ~(1 << bit);
    spi_shiftout_write(matrix->led_matrix_cfg);
}

void led_matrix_clear(led_matrix_t *matrix)
{
    if (matrix == NULL) {
        ESP_LOGE("led_matrix", "LED matrix or configuration is NULL");
        return;
    }
    memset(matrix->led_matrix, 0xff, (matrix->width + 7u) / 8u * matrix->height);
}

void led_matrix_draw_sync(led_matrix_t *matrix, SemaphoreHandle_t syncMutex, led_matrix_draw_func_t drawFunc, void *data)
{
    while (xSemaphoreTake(syncMutex, 0) != pdTRUE) {
        // Wait for the semaphore to be available
        vTaskDelay(1);
    }
    drawFunc(matrix, data);
    xSemaphoreGive(syncMutex);
}
