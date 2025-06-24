#include "esp_err.h"
#include "esp_log.h"
#include "led_matrix.h"
#include <stdint.h>
#include "led_matrix.h"

#ifdef __cplusplus
extern "C"
{
#endif

void draw_digit_3x(led_matrix_t *matrix, uint8_t digit, uint8_t x, uint8_t y);

#ifdef __cplusplus
}
#endif