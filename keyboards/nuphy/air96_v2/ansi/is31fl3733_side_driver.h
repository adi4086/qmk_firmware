
#include "is31fl3733.h"

#pragma once

#define    SIDE_LINE           5
#define    SIDE_INDEX          100
#define    LEFT_SIDE           1
#define    RIGHT_SIDE          2


typedef struct is31fl3733_driver_t {
    uint8_t pwm_buffer[192];
    bool    pwm_buffer_dirty;
    uint8_t led_control_buffer[24];
    bool    led_control_buffer_dirty;
} PACKED is31fl3733_driver_t;

extern is31fl3733_driver_t driver_buffers[DRIVER_COUNT];
bool is_side_is31fl3733_off(void);
void rgb_matrix_set_color(int index, uint8_t red, uint8_t green, uint8_t blue);
void side_is31fl3733_set_color_strip(uint8_t side, uint8_t r, uint8_t g, uint8_t b);
