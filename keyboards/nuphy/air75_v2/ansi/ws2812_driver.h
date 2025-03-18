#include "ws2812.h"
#include "gpio.h"
#include "stdbool.h"
#include "chibios_config.h"
#include "color.h"

/*
Copyright 2023 @ Nuphy <https://nuphy.com/>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifdef WS2812_TIMING
#    undef WS2812_TIMING //1250
#endif

#ifdef WS2812_T1H
#    undef WS2812_T1H    // 900
#endif

#ifdef WS2812_T1L
#    undef WS2812_T1L    // 350
#endif

#ifdef WS2812_T0H
#    undef WS2812_T0H
#endif

#ifdef WS2812_T0L
#    undef WS2812_T0L
#endif

#define WS2812_TIMING 850
#define WS2812_T1H 700 // Width of a 1 bit in ns
#define WS2812_T1L (WS2812_TIMING - WS2812_T1H) // Width of a 1 bit in ns
#define WS2812_T0H 150 // Width of a 0 bit in ns
#define WS2812_T0L (WS2812_TIMING - WS2812_T0H) // Width of a 0 bit in ns


// DEPRECATED - DO NOT USE
#if defined(NOP_FUDGE)
#    define WS2812_BITBANG_NOP_FUDGE NOP_FUDGE
#endif

/* Adapted from https://github.com/bigjosh/SimpleNeoPixelDemo/ */

#ifndef WS2812_BITBANG_NOP_FUDGE
#    if defined(STM32F0XX) || defined(STM32F1XX) || defined(GD32VF103) || defined(STM32F3XX) || defined(STM32F4XX) || defined(STM32L0XX) || defined(WB32F3G71xx) || defined(WB32FQ95xx) || defined(AT32F415)
#        define WS2812_BITBANG_NOP_FUDGE 0.4
#    else
#        if defined(RP2040)
#            error "Please use `vendor` WS2812 driver for RP2040"
#        else
#            error "WS2812_BITBANG_NOP_FUDGE configuration required"
#        endif
#        define WS2812_BITBANG_NOP_FUDGE 1 // this just pleases the compile so the above error is easier to spot
#    endif
#endif

// Push Pull or Open Drain Configuration
// Default Push Pull
#ifndef WS2812_EXTERNAL_PULLUP
#    define WS2812_OUTPUT_MODE PAL_MODE_OUTPUT_PUSHPULL
#else
#    define WS2812_OUTPUT_MODE PAL_MODE_OUTPUT_OPENDRAIN
#endif

// The reset gap can be 6000 ns, but depending on the LED strip it may have to be increased
// to values like 600000 ns. If it is too small, the pixels will show nothing most of the time.
#ifndef WS2812_RES
#    define WS2812_RES (1000 * WS2812_TRST_US) // Width of the low gap between bits to cause a frame to latch
#endif

#define NUMBER_NOPS 6
#define CYCLES_PER_SEC (CPU_CLOCK / NUMBER_NOPS * WS2812_BITBANG_NOP_FUDGE)
#define NS_PER_SEC (1000000000L) // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives
#define NS_PER_CYCLE (NS_PER_SEC / CYCLES_PER_SEC)
#define NS_TO_CYCLES(n) ((n) / NS_PER_CYCLE)

#define wait_ns(x)                                  \
    do {                                            \
        for (int i = 0; i < NS_TO_CYCLES(x); i++) { \
            __asm__ volatile("nop\n\t"              \
                             "nop\n\t"              \
                             "nop\n\t"              \
                             "nop\n\t"              \
                             "nop\n\t"              \
                             "nop\n\t");            \
        }                                           \
    } while (0)


#define    LEFT_SIDE           1
#define    RIGHT_SIDE          2

#define    SIDE_LINE           6
#define    SIDE_LED_NUM        12


extern   rgb_t           side_leds[SIDE_LED_NUM];
extern   bool            flush_side_leds;

extern   ws2812_led_t    ws2812_leds[WS2812_LED_COUNT];
extern   bool            flush_rgb_leds;


void     side_sendByte(uint8_t byte);
bool     is_side_ws2812_off(void);
void     side_ws2812_set_color(int i, uint8_t r, uint8_t g, uint8_t b);
void     side_ws2812_set_color_strip(uint8_t side, uint8_t r, uint8_t g, uint8_t b);
void     side_ws2812_flush(void);

