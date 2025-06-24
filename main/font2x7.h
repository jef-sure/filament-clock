#include <stdint.h>
#include "led_matrix.h"
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

void draw_digit_2x(led_matrix_t *matrix, uint8_t digit, uint8_t x, uint8_t y);

#ifdef __cplusplus
}
#endif