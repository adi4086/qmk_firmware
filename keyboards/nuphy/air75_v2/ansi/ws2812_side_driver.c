#include "ws2812_driver.h"

rgb_t    side_leds[SIDE_LED_NUM] = {0};
bool     flush_side_leds         = 0;

void side_sendByte(uint8_t byte) {
    // WS2812 protocol wants most significant bits first
    for (unsigned char bit = 0; bit < 8; bit++) {
        bool is_one = byte & (1 << (7 - bit));
        // using something like wait_ns(is_one ? T1L : T0L) here throws off timings
        if (is_one) {
            // 1
            gpio_write_pin_high(DRIVER_SIDE_PIN);
            wait_ns(WS2812_T1H);
            gpio_write_pin_low(DRIVER_SIDE_PIN);
            wait_ns(WS2812_T1L);
        } else {
            // 0
            gpio_write_pin_high(DRIVER_SIDE_PIN);
            wait_ns(WS2812_T0H);
            gpio_write_pin_low(DRIVER_SIDE_PIN);
            wait_ns(WS2812_T0L);
        }
    }
}

bool is_side_ws2812_off(void) {
    for (int i = 0; i < SIDE_LED_NUM; i++) {
        if ((side_leds[i].r != 0) || (side_leds[i].g != 0) || (side_leds[i].b != 0)) {
            return false;
        }
    }
    return true;
}

void side_ws2812_set_color(int i, uint8_t r, uint8_t g, uint8_t b) {
    r >>= 2, g >>= 2, b >>= 2;
    if (side_leds[i].r != r || side_leds[i].g != g || side_leds[i].b != b) {
        flush_side_leds = true;
    }
    side_leds[i].r = r;
    side_leds[i].g = g;
    side_leds[i].b = b;
}

void side_ws2812_set_color_strip(uint8_t side, uint8_t r, uint8_t g, uint8_t b) {
    // side = 1 => left
    // side = 2 => right
    // side = 3 => both
    uint8_t start = 0;
    uint8_t end   = SIDE_LED_NUM;
    if (side == LEFT_SIDE)  { end = end - SIDE_LINE; }
    if (side == RIGHT_SIDE) { start = start + SIDE_LINE; }

    for (uint8_t i = start; i < end; i++) {
        side_ws2812_set_color(i, r, g, b);
    }
}

void side_ws2812_flush(void) {
    if (!flush_side_leds) { return; }

    // this code is very time dependent, so we need to disable interrupts
    chSysLock();

    for (int i = 0; i < SIDE_LED_NUM; i++) {
        // WS2812 protocol dictates grb order
        side_sendByte(side_leds[i].g);
        side_sendByte(side_leds[i].r);
        side_sendByte(side_leds[i].b);
    }

    wait_ns(WS2812_RES);

    chSysUnlock();

    flush_side_leds = false;
}
