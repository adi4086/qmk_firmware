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

#include QMK_KEYBOARD_H
#include "layer_names.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

// layer 0 Mac
[MAC_BASE] = LAYOUT_96_ansi(
   KC_ESC,    KC_BRID,   KC_BRIU,   TASK,        SEARCH,       MAC_VOICE, MAC_DND,   KC_MPRV,     KC_MPLY,   KC_MNXT,    KC_MUTE,    KC_VOLD,    KC_VOLU,     PRT_AREA,  KC_DEL,     KC_HOME,   KC_END,    KC_PGUP,   KC_PGDN,
   KC_GRV,    KC_1,      KC_2,      KC_3,        KC_4,         KC_5,      KC_6,      KC_7,        KC_8,      KC_9,       KC_0,       KC_MINS,    KC_EQL,      KC_BSPC,               KC_NUM,    KC_PSLS,   KC_PAST,   KC_PMNS,
   KC_TAB,    KC_Q,      KC_W,      KC_E,        KC_R,         KC_T,      KC_Y,      KC_U,        KC_I,      KC_O,       KC_P,       KC_LBRC,    KC_RBRC,     KC_BSLS,               KC_P7,     KC_P8,     KC_P9,     KC_PPLS,
   KC_CAPS,   KC_A,      KC_S,      KC_D,        KC_F,         KC_G,      KC_H,      KC_J,        KC_K,      KC_L,       KC_SCLN,    KC_QUOT,    KC_ENT,                             KC_P4,     KC_P5,     KC_P6,
   KC_LSFT,              KC_Z,      KC_X,        KC_C,         KC_V,      KC_B,      KC_N,        KC_M,      KC_COMM,    KC_DOT,     KC_SLSH,    KC_RSFT,                KC_UP,      KC_P1,     KC_P2,     KC_P3,     KC_PENT,
   KC_LCTL,   KC_LALT,   KC_LGUI,                              KC_SPC,                                                   KC_RGUI,    MO(1),      KC_RCTL,     KC_LEFT,   KC_DOWN,    KC_RGHT,   KC_P0,     KC_PDOT),
// layer 1 Mac Fn
[MAC_FN] = LAYOUT_96_ansi(
   _______,   KC_F1,     KC_F2,     KC_F3,       KC_F4,        KC_F5,     KC_F6,     KC_F7,       KC_F8,     KC_F9,      KC_F10,     KC_F11,     KC_F12,      PRT_SCR,   KC_INS,     _______,   _______,   _______,   _______,
   _______,   LNK_BLE1,  LNK_BLE2,  LNK_BLE3,    LNK_RF,       _______,   _______,   _______,     _______,   _______,    _______,    _______,    _______,     _______,               _______,   _______,   _______,   _______,
   _______,   _______,   _______,   _______,     _______,      _______,   _______,   _______,     _______,   _______,    _______,    DEV_RESET,  SLEEP_MODE,  BAT_SHOW,              _______,   _______,   _______,   _______,
   _______,   _______,   _______,   _______,     _______,      _______,   _______,   _______,     _______,   _______,    _______,    _______,    _______,                            _______,   _______,   _______,
   _______,              _______,   _______,     RGB_TEST,     _______,   BAT_NUM,   _______,     MO(4),     RM_SPDD,    RM_SPDU,    _______,    _______,                RM_VALU,    _______,   _______,   _______,   _______,
   _______,   _______,   _______,                              _______,                                                  _______,    _______,    _______,     RM_NEXT,   RM_VALD,    RM_HUEU,   _______,   _______),
// layer 2 win
[WIN_BASE] = LAYOUT_96_ansi(
   KC_ESC,    KC_F1,     KC_F2,     KC_F3,       KC_F4,        KC_F5,     KC_F6,     KC_F7,       KC_F8,     KC_F9,      KC_F10,     KC_F11,     KC_F12,      KC_PSCR,   KC_DEL,      KC_HOME,  KC_END,    KC_PGUP,   KC_PGDN,
   KC_GRV,    KC_1,      KC_2,      KC_3,        KC_4,         KC_5,      KC_6,      KC_7,        KC_8,      KC_9,       KC_0,       KC_MINS,    KC_EQL,      KC_BSPC,                KC_NUM,   KC_PSLS,   KC_PAST,   KC_PMNS,
   KC_TAB,    KC_Q,      KC_W,      KC_E,        KC_R,         KC_T,      KC_Y,      KC_U,        KC_I,      KC_O,       KC_P,       KC_LBRC,    KC_RBRC,     KC_BSLS,                KC_P7,    KC_P8,     KC_P9,     KC_PPLS,
   KC_CAPS,   KC_A,      KC_S,      KC_D,        KC_F,         KC_G,      KC_H,      KC_J,        KC_K,      KC_L,       KC_SCLN,    KC_QUOT,    KC_ENT,                              KC_P4,    KC_P5,     KC_P6,
   KC_LSFT,              KC_Z,      KC_X,        KC_C,         KC_V,      KC_B,      KC_N,        KC_M,      KC_COMM,    KC_DOT,     KC_SLSH,    KC_RSFT,                KC_UP,       KC_P1,    KC_P2,     KC_P3,     KC_PENT,
   KC_LCTL,   KC_LGUI,   KC_LALT,                              KC_SPC,                                                   KC_RALT,    MO(3),      KC_RCTL,     KC_LEFT,   KC_DOWN,     KC_RGHT,  KC_P0,     KC_PDOT),
// layer 3 win Fn
[WIN_FN] = LAYOUT_96_ansi(
   _______,   KC_BRID,   KC_BRIU,   KC_CALC,     SEARCH,       KC_F5,     KC_F6,     KC_MPRV,     KC_MPLY,   KC_MNXT,    KC_MUTE,    KC_VOLD,    KC_VOLU,     _______,   KC_INS,      _______,  _______,   _______,   _______,
   _______,   LNK_BLE1,  LNK_BLE2,  LNK_BLE3,    LNK_RF,       _______,   _______,   _______,     _______,   _______,    _______,    _______,    _______,     _______,                _______,  _______,   _______,   _______,
   _______,   _______,   _______,   _______,     _______,      _______,   _______,   _______,     _______,   _______,    _______,    DEV_RESET,  SLEEP_MODE,  BAT_SHOW,               _______,  _______,   _______,   _______,
   _______,   _______,   _______,   _______,     _______,      _______,   _______,   _______,     _______,   _______,    _______,    _______,    _______,                             _______,  _______,   _______,
   _______,              _______,   _______,     RGB_TEST,     _______,   BAT_NUM,   _______,     MO(4),     RM_SPDD,    RM_SPDU,    _______,    _______,                RM_VALU,     _______,  _______,   _______,   _______,
   _______,   _______,   _______,                              _______,                                                  _______,    _______,    _______,     RM_NEXT,   RM_VALD,     RM_HUEU,  _______,   _______),
// layer 4 function
[M_LAYER] = LAYOUT_96_ansi(
   QK_REBOOT, DB_TOGG,   RF_DFU,    _______,     _______,      _______,   _______,   _______,     _______,   _______,    _______,    DEBOUNCE_D,  DEBOUNCE_I, _______,   _______,     _______,   _______,   _______,   _______,
   _______,   _______,   _______,   _______,     _______,      _______,   _______,   _______,     _______,   _______,    _______,    SLEEP_D,    SLEEP_I,     _______,                _______,   _______,   _______,   _______,
   _______,   _______,   _______,   _______,     _______,      _______,   _______,   _______,     _______,   SIDE_1,     _______,    _______,    SLEEP_NOW,   _______,                _______,   _______,   _______,   _______,
   CAPS_WORD, _______,   _______,   DEBOUNCE_T,  _______,      GAME_MODE, _______,   _______,     SOCD_TOG,  _______,    _______,    _______,    _______,                             _______,   _______,   _______,
   _______,              _______,   _______,     _______,      _______,   _______,   NUMLOCK_IND, _______,   SIDE_SPD,   SIDE_SPI,   _______,    _______,                SIDE_VAI,    _______,   _______,   _______,   _______,
   _______,   _______,   _______,                              _______,                                                  _______,    MO(4),      _______,     SIDE_MOD,  SIDE_VAD,    SIDE_HUI,  _______,   _______)
};

const is31fl3733_led_t PROGMEM g_is31fl3733_leds[RGB_MATRIX_LED_COUNT] = {
    {0, SW1_CS16,   SW2_CS16,   SW3_CS16},
    {0, SW1_CS15,   SW2_CS15,   SW3_CS15},
    {0, SW1_CS14,   SW2_CS14,   SW3_CS14},
    {0, SW1_CS13,   SW2_CS13,   SW3_CS13},
    {0, SW1_CS12,   SW2_CS12,   SW3_CS12},
    {0, SW4_CS16,   SW5_CS16,   SW6_CS16},
    {0, SW4_CS15,   SW5_CS15,   SW6_CS15},
    {0, SW4_CS14,   SW5_CS14,   SW6_CS14},
    {0, SW4_CS13,   SW5_CS13,   SW6_CS13},
    {0, SW4_CS12,   SW5_CS12,   SW6_CS12},
    {1, SW4_CS16,   SW5_CS16,   SW6_CS16},
    {1, SW4_CS15,   SW5_CS15,   SW6_CS15},
    {1, SW4_CS14,   SW5_CS14,   SW6_CS14},
    {1, SW4_CS13,   SW5_CS13,   SW6_CS13},
    {1, SW7_CS4,    SW8_CS4,    SW9_CS4},
    {1, SW4_CS5,    SW5_CS5,    SW6_CS5},
    {1, SW7_CS5,    SW8_CS5,    SW9_CS5},
    {1, SW4_CS6,    SW5_CS6,    SW6_CS6},
    {1, SW7_CS6,    SW8_CS6,    SW9_CS6},
    {0, SW1_CS1,    SW2_CS1,    SW3_CS1},
    {0, SW1_CS2,    SW2_CS2,    SW3_CS2},
    {0, SW1_CS3,    SW2_CS3,    SW3_CS3},
    {0, SW1_CS4,    SW2_CS4,    SW3_CS4},
    {0, SW1_CS5,    SW2_CS5,    SW3_CS5},
    {0, SW1_CS6,    SW2_CS6,    SW3_CS6},
    {0, SW1_CS7,    SW2_CS7,    SW3_CS7},
    {0, SW1_CS8,    SW2_CS8,    SW3_CS8},
    {0, SW1_CS9,    SW2_CS9,    SW3_CS9},
    {0, SW1_CS10,   SW2_CS10,   SW3_CS10},
    {0, SW1_CS11,   SW2_CS11,   SW3_CS11},
    {1, SW4_CS1,    SW5_CS1,    SW6_CS1},
    {1, SW4_CS2,    SW5_CS2,    SW6_CS2},
    {1, SW4_CS3,    SW5_CS3,    SW6_CS3},
    {1, SW4_CS7,    SW5_CS7,    SW6_CS7},
    {1, SW4_CS8,    SW5_CS8,    SW6_CS8},
    {1, SW4_CS9,    SW5_CS9,    SW6_CS9},
    {1, SW4_CS10,   SW5_CS10,   SW6_CS10},
    {0, SW4_CS1,    SW5_CS1,    SW6_CS1},
    {0, SW4_CS2,    SW5_CS2,    SW6_CS2},
    {0, SW4_CS3,    SW5_CS3,    SW6_CS3},
    {0, SW4_CS4,    SW5_CS4,    SW6_CS4},
    {0, SW4_CS5,    SW5_CS5,    SW6_CS5},
    {0, SW4_CS6,    SW5_CS6,    SW6_CS6},
    {0, SW4_CS7,    SW5_CS7,    SW6_CS7},
    {0, SW4_CS8,    SW5_CS8,    SW6_CS8},
    {0, SW4_CS9,    SW5_CS9,    SW6_CS9},
    {0, SW4_CS10,   SW5_CS10,   SW6_CS10},
    {0, SW4_CS11,   SW5_CS11,   SW6_CS11},
    {1, SW7_CS1,    SW8_CS1,    SW9_CS1},
    {1, SW7_CS2,    SW8_CS2,    SW9_CS2},
    {1, SW7_CS3,    SW8_CS3,    SW9_CS3},
    {1, SW7_CS7,    SW8_CS7,    SW9_CS7},
    {1, SW7_CS8,    SW8_CS8,    SW9_CS8},
    {1, SW7_CS9,    SW8_CS9,    SW9_CS9},
    {1, SW7_CS10,   SW8_CS10,   SW9_CS10},
    {0, SW7_CS1,    SW8_CS1,    SW9_CS1},
    {0, SW7_CS2,    SW8_CS2,    SW9_CS2},
    {0, SW7_CS3,    SW8_CS3,    SW9_CS3},
    {0, SW7_CS4,    SW8_CS4,    SW9_CS4},
    {0, SW7_CS5,    SW8_CS5,    SW9_CS5},
    {0, SW7_CS6,    SW8_CS6,    SW9_CS6},
    {0, SW7_CS7,    SW8_CS7,    SW9_CS7},
    {0, SW7_CS8,    SW8_CS8,    SW9_CS8},
    {0, SW7_CS9,    SW8_CS9,    SW9_CS9},
    {0, SW7_CS10,   SW8_CS10,   SW9_CS10},
    {0, SW7_CS11,   SW8_CS11,   SW9_CS11},
    {1, SW7_CS16,   SW8_CS16,   SW9_CS16},
    {1, SW7_CS14,   SW8_CS14,   SW9_CS14},
    {1, SW7_CS13,   SW8_CS13,   SW9_CS13},
    {1, SW7_CS12,   SW8_CS12,   SW9_CS12},
    {1, SW7_CS11,   SW8_CS11,   SW9_CS11},
    {0, SW10_CS1,    SW11_CS1,    SW12_CS1},
    {0, SW10_CS3,    SW11_CS3,    SW12_CS3},
    {0, SW10_CS4,    SW11_CS4,    SW12_CS4},
    {0, SW10_CS5,    SW11_CS5,    SW12_CS5},
    {0, SW10_CS6,    SW11_CS6,    SW12_CS6},
    {0, SW10_CS7,    SW11_CS7,    SW12_CS7},
    {0, SW10_CS8,    SW11_CS8,    SW12_CS8},
    {0, SW10_CS9,    SW11_CS9,    SW12_CS9},
    {0, SW10_CS10,   SW11_CS10,   SW12_CS10},
    {0, SW10_CS11,   SW11_CS11,   SW12_CS11},
    {1, SW10_CS1,    SW11_CS1,    SW12_CS1},
    {1, SW10_CS3,    SW11_CS3,    SW12_CS3},
    {1, SW10_CS4,    SW11_CS4,    SW12_CS4},
    {1, SW10_CS5,    SW11_CS5,    SW12_CS5},
    {1, SW10_CS6,    SW11_CS6,    SW12_CS6},
    {1, SW10_CS7,    SW11_CS7,    SW12_CS7},
    {1, SW10_CS8,    SW11_CS8,    SW12_CS8},
    {0, SW10_CS16,   SW11_CS16,   SW12_CS16},
    {0, SW10_CS15,   SW11_CS15,   SW12_CS15},
    {0, SW10_CS14,   SW11_CS14,   SW12_CS14},
    {0, SW10_CS13,   SW11_CS13,   SW12_CS13},
    {0, SW10_CS12,   SW11_CS12,   SW12_CS12},
    {1, SW10_CS16,   SW11_CS16,   SW12_CS16},
    {1, SW10_CS14,   SW11_CS14,   SW12_CS14},
    {1, SW10_CS13,   SW11_CS13,   SW12_CS13},
    {1, SW10_CS12,   SW11_CS12,   SW12_CS12},
    {1, SW10_CS11,   SW11_CS11,   SW12_CS11},
    {1, SW10_CS10,   SW11_CS10,   SW12_CS10},
    {1, SW10_CS9,    SW11_CS9,    SW12_CS9},

    {1, SW1_CS5,    SW2_CS5,    SW3_CS5},
    {1, SW1_CS4,    SW2_CS4,    SW3_CS4},
    {1, SW1_CS3,    SW2_CS3,    SW3_CS3},
    {1, SW1_CS2,    SW2_CS2,    SW3_CS2},
    {1, SW1_CS1,    SW2_CS1,    SW3_CS1},

    {1, SW1_CS6,    SW2_CS6,    SW3_CS6},
    {1, SW1_CS7,    SW2_CS7,    SW3_CS7},
    {1, SW1_CS8,    SW2_CS8,    SW3_CS8},
    {1, SW1_CS9,    SW2_CS9,    SW3_CS9},
    {1, SW1_CS10,   SW2_CS10,   SW3_CS10}
};

