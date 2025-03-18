#include "ws2812_driver.h"


ws2812_led_t ws2812_leds[WS2812_LED_COUNT];
bool flush_rgb_leds = 0;

void sendByte(uint8_t byte) {
    // WS2812 protocol wants most significant bits first
    for (unsigned char bit = 0; bit < 8; bit++) {
        bool is_one = byte & (1 << (7 - bit));
        // using something like wait_ns(is_one ? T1L : T0L) here throws off timings
        if (is_one) {
            // 1
            gpio_write_pin_high(WS2812_DI_PIN);
            wait_ns(WS2812_T1H);
            gpio_write_pin_low(WS2812_DI_PIN);
            wait_ns(WS2812_T1L);
        } else {
            // 0
            gpio_write_pin_high(WS2812_DI_PIN);
            wait_ns(WS2812_T0H);
            gpio_write_pin_low(WS2812_DI_PIN);
            wait_ns(WS2812_T0L);
        }
    }
}

void ws2812_init(void) {
    palSetLineMode(WS2812_DI_PIN, WS2812_OUTPUT_MODE);
}

void ws2812_set_color(int index, uint8_t red, uint8_t green, uint8_t blue) {
    if (ws2812_leds[index].r != red || ws2812_leds[index].g != green || ws2812_leds[index].b != blue) {
        flush_rgb_leds = true;
    }
    ws2812_leds[index].r = red;
    ws2812_leds[index].g = green;
    ws2812_leds[index].b = blue;
#if defined(WS2812_RGBW)
    ws2812_rgb_to_rgbw(&ws2812_leds[index]);
#endif
}

void ws2812_set_color_all(uint8_t red, uint8_t green, uint8_t blue) {
    for (int i = 0; i < WS2812_LED_COUNT; i++) {
        ws2812_set_color(i, red, green, blue);
    }
}

void ws2812_flush(void) {
    if (!flush_rgb_leds) { return; }
    // this code is very time dependent, so we need to disable interrupts
    chSysLock();

    for (int i = 0; i < WS2812_LED_COUNT; i++) {
        // WS2812 protocol dictates grb order
#if (WS2812_BYTE_ORDER == WS2812_BYTE_ORDER_GRB)
        sendByte(ws2812_leds[i].g);
        sendByte(ws2812_leds[i].r);
        sendByte(ws2812_leds[i].b);
#elif (WS2812_BYTE_ORDER == WS2812_BYTE_ORDER_RGB)
        sendByte(ws2812_leds[i].r);
        sendByte(ws2812_leds[i].g);
        sendByte(ws2812_leds[i].b);
#elif (WS2812_BYTE_ORDER == WS2812_BYTE_ORDER_BGR)
        sendByte(ws2812_leds[i].b);
        sendByte(ws2812_leds[i].g);
        sendByte(ws2812_leds[i].r);
#endif

#ifdef WS2812_RGBW
        sendByte(ws2812_leds[i].w);
#endif
    }

    wait_ns(WS2812_RES);

    chSysUnlock();

    flush_rgb_leds = false;
}
