#include "font3x5.h"
#include "esp_err.h"
#include "esp_log.h"
#include "led_matrix.h"
#include <stdint.h>

const uint8_t IMAGES[][3] = {
{
  0b00001110,
  0b00010001,
  0b00001110,
},{
  0b00010010,
  0b00011111,
  0b00010000,
},{
  0b00011001,
  0b00010101,
  0b00010010,
},{
  0b00010001,
  0b00010101,
  0b00001010,
},{
  0b00000111,
  0b00000100,
  0b00011110,
},{
  0b00010111,
  0b00010101,
  0b00001001,
},{
  0b00001110,
  0b00010101,
  0b00001001,
},{
  0b00011001,
  0b00000101,
  0b00000011,
},{
  0b00001010,
  0b00010101,
  0b00001010,
},{
  0b00010010,
  0b00010101,
  0b00001110,
}};


void draw_digit_3x5(led_matrix_t *matrix, uint8_t digit, uint8_t x, uint8_t y)
{
    if (digit >= 10) {
        ESP_LOGE("draw_digit", "Digit out of bounds: %d", digit);
        return;
    }
    if (x >= matrix->width || y >= matrix->height) {
        ESP_LOGE("draw_digit", "Coordinates out of bounds: (%d, %d)", x, y);
        return;
    }
    for (uint8_t i = 0; i < 3; i++) {
        for (uint8_t j = 3; j < 8; j++) {
            led_matrix_set(matrix, x + i, y + (7 - j), (IMAGES[digit][i] >> (7 - j)) & 1);
        }
    }
}