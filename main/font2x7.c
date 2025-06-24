#include "esp_err.h"
#include "esp_log.h"
#include "led_matrix.h"

const static uint8_t IMAGES[][2] = {
    {
     0b01111111,         //
        0b01111111,            //
    },
    {
     0b00000010,                               //
        0b01111111,                                         //
    },
    {
     0b01110001, //
        0b01001111,            //
    },
    {
     0b01001001,                               //
        0b01110111,                                         //
    },
    {
     0b00001111,                   //
        0b01111100,            //
    },
    {
     0b01001111,                               //
        0b01111001, //
    },
    {
     0b01111111, //
        0b01111001, //
    },
    {
     0b01111001, //
        0b00000111, //
    },
    {
     0b01110111, //
        0b01110111, //
    },
    {
     0b01000111, //
        0b01111111, //
    }
};

void draw_digit_2x(led_matrix_t *matrix, uint8_t digit, uint8_t x, uint8_t y)
{
    if (digit >= 10) {
        ESP_LOGE("draw_digit", "Digit out of bounds: %d", digit);
        return;
    }
    if (x >= matrix->width || y >= matrix->height) {
        ESP_LOGE("draw_digit", "Coordinates out of bounds: (%d, %d)", x, y);
        return;
    }
    for (uint8_t i = 0; i < 2; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            led_matrix_set(matrix, x + i, y + (7 - j), (IMAGES[digit][i] >> (7 - j)) & 1);
        }
    }
}