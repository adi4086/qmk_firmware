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

#define DEV_MODE_PIN                        C0
#define SYS_MODE_PIN                        C1
#define DC_BOOST_PIN                        C2
#define NRF_RESET_PIN                       B4
#define NRF_TEST_PIN                        B5
#define NRF_WAKEUP_PIN                      B8
#define DRIVER_LED_CS_PIN                   C6
#define DRIVER_SIDE_PIN                     C8
#define DRIVER_SIDE_CS_PIN                  C9

#define SERIAL_DRIVER                       SD1
#define SD1_TX_PIN                          B6
#define SD1_TX_PAL_MODE                     0
#define SD1_RX_PIN                          B7
#define SD1_RX_PAL_MODE                     0

#if !defined(NO_DEBUG) && !defined(CONSOLE_ENABLE)
#define NO_DEBUG
#endif // !NO_DEBUG

#if !defined(NO_PRINT) && !defined(CONSOLE_ENABLE)
#define NO_PRINT
#endif // !NO_PRINT
 
#if !defined(NO_DEBUG) && defined(CONSOLE_ENABLE)
#define DEBUG_MATRIX_SCAN_RATE
#endif // DEBUG_MATRIX_SCAN_RATE
 
#define DYNAMIC_KEYMAP_MACRO_DELAY          8

#define WAIT_US_TIMER                       GPTD14
// #undef USE_CIE1931_CURVE

/*
#define WS2812_PWM_DRIVER PWMD3
#define WS2812_PWM_CHANNEL 2
#define WS2812_PWM_PAL_MODE 1
#define WS2812_DMA_STREAM STM32_DMA1_STREAM3
#define WS2812_DMA_CHANNEL 3
*/
// #define WS2812_PWM_TARGET_PERIOD 800000


#define WS2812_SPI_USE_CIRCULAR_BUFFER

#define EECONFIG_KB_DATA_SIZE               26
#define GPIO_INPUT_PIN_DELAY                12
#define RF_QUEUE_SIZE                       64

#define OS_DETECTION_DEBOUNCE              250
#define OS_DETECTION_KEYBOARD_RESET

// remove unused QMK functions to save space
/*
#define NO_ACTION_ONESHOT
#undef LAYER_LOCK_ENABLE
#undef LEADER_ENABLE
#undef GRAVE_ESC_ENABLE
#undef SPACE_CADET_ENABLE
*/
