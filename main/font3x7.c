#include "font3x7.h"
#include "esp_err.h"
#include "esp_log.h"
#include "led_matrix.h"
#include <stdint.h>

const static uint8_t IMAGES[][8] = {
{
  0b00111110,
  0b01000001,
  0b00111110,
},{
  0b00000100,
  0b00000010,
  0b01111111,
},{
  0b01100010,
  0b01010001,
  0b01001110,
},{
  0b01000001,
  0b01001001,
  0b00110110,
},{
  0b00001111,
  0b00001000,
  0b01111110,
},{
  0b01001111,
  0b01001001,
  0b00110001,
},{
  0b00111110,
  0b01001001,
  0b00110001,
},{
  0b01111001,
  0b00000101,
  0b00000011,
},{
  0b00110110,
  0b01001001,
  0b00110110,
},{
  0b01000110,
  0b01001001,
  0b00111110,
}};

void draw_digit_3x(led_matrix_t *matrix, uint8_t digit, uint8_t x, uint8_t y)
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
        for (uint8_t j = 0; j < 8; j++) {
            led_matrix_set(matrix, x + i, y + (7 - j), (IMAGES[digit][i] >> (7 - j)) & 1);
        }
    }
}