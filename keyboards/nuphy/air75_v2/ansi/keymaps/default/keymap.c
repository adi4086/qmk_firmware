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

// layer Mac
[MAC_BASE] = LAYOUT_75_ansi(
    KC_ESC,     KC_BRID,   KC_BRIU,     TASK,       SEARCH,     MAC_VOICE,   MAC_DND,   KC_MPRV,     KC_MPLY,     KC_MNXT,   KC_MUTE,    KC_VOLD,    KC_VOLU,     PRT_AREA,   KC_INS,     KC_DEL,
    KC_GRV,     KC_1,      KC_2,        KC_3,       KC_4,       KC_5,        KC_6,      KC_7,        KC_8,        KC_9,      KC_0,       KC_MINS,    KC_EQL,                  KC_BSPC,    KC_PGUP,
    KC_TAB,     KC_Q,      KC_W,        KC_E,       KC_R,       KC_T,        KC_Y,      KC_U,        KC_I,        KC_O,      KC_P,       KC_LBRC,    KC_RBRC,                 KC_BSLS,    KC_PGDN,
    KC_CAPS,    KC_A,      KC_S,        KC_D,       KC_F,       KC_G,        KC_H,      KC_J,        KC_K,        KC_L,      KC_SCLN,    KC_QUOT,                             KC_ENT,     KC_HOME,
    KC_LSFT,               KC_Z,        KC_X,       KC_C,       KC_V,        KC_B,      KC_N,        KC_M,        KC_COMM,   KC_DOT,     KC_SLSH,                 KC_RSFT,    KC_UP,      KC_END,
    KC_LCTL,    KC_LALT,   KC_LGUI,                                          KC_SPC,                              KC_RGUI,   MO(1),      KC_RCTL,                 KC_LEFT,    KC_DOWN,    KC_RGHT),

// layer Mac Fn
[MAC_FN] = LAYOUT_75_ansi(
    _______,    KC_F1,      KC_F2,      KC_F3,      KC_F4,      KC_F5,       KC_F6,     KC_F7,       KC_F8,       KC_F9,     KC_F10,     KC_F11,     KC_F12,      PRT_SCR,    _______,    _______,
    _______,    LNK_BLE1,   LNK_BLE2,   LNK_BLE3,   LNK_RF,     _______,     _______,   _______,     _______,     _______,   _______,    _______,    _______,                 _______,    _______,
    _______,    _______,    _______,    _______,    _______,    _______,     _______,   _______,     _______,     _______,   _______,    DEV_RESET,  SLEEP_MODE,              BAT_SHOW,   _______,
    _______,    _______,    _______,    _______,    _______,    _______,     _______,   _______,     _______,     _______,   _______,    _______,                             _______,    _______,
    _______,                _______,    _______,    RGB_TEST,   _______,     BAT_NUM,   _______,     MO(4),       RGB_SPD,   RGB_SPI,    _______,                 _______,    RGB_VAI,    _______,
    _______,    _______,    _______,                                         _______,                             _______,   MO(1),      _______,                 RGB_MOD,    RGB_VAD,    RGB_HUI),

// layer win
[WIN_BASE] = LAYOUT_75_ansi(
    KC_ESC,     KC_F1,     KC_F2,       KC_F3,      KC_F4,      KC_F5,       KC_F6,     KC_F7,       KC_F8,       KC_F9,     KC_F10,     KC_F11,     KC_F12,      KC_PSCR,    KC_INS,     KC_DEL,
    KC_GRV,     KC_1,      KC_2,        KC_3,       KC_4,       KC_5,        KC_6,      KC_7,        KC_8,        KC_9,      KC_0,       KC_MINS,    KC_EQL,                  KC_BSPC,    KC_PGUP,
    KC_TAB,     KC_Q,      KC_W,        KC_E,       KC_R,       KC_T,        KC_Y,      KC_U,        KC_I,        KC_O,      KC_P,       KC_LBRC,    KC_RBRC,                 KC_BSLS,    KC_PGDN,
    KC_CAPS,    KC_A,      KC_S,        KC_D,       KC_F,       KC_G,        KC_H,      KC_J,        KC_K,        KC_L,      KC_SCLN,    KC_QUOT,                             KC_ENT,     KC_HOME,
    KC_LSFT,               KC_Z,        KC_X,       KC_C,       KC_V,        KC_B,      KC_N,        KC_M,        KC_COMM,   KC_DOT,     KC_SLSH,                 KC_RSFT,    KC_UP,      KC_END,
    KC_LCTL,    KC_LGUI,   KC_LALT,                                          KC_SPC,                              KC_RALT,   MO(3),      KC_RCTL,                 KC_LEFT,    KC_DOWN,    KC_RGHT),

// layer win Fn
[WIN_FN] = LAYOUT_75_ansi(
    _______,    KC_BRID,    KC_BRIU,    KC_CALC,    SEARCH,     _______,     MAC_DND,   KC_MPRV,     KC_MPLY,     KC_MNXT,   KC_MUTE,    KC_VOLD,    KC_VOLU,     _______,    _______,    _______,
    _______,    LNK_BLE1,   LNK_BLE2,   LNK_BLE3,   LNK_RF,     _______,     _______,   _______,     _______,     _______,   _______,    _______,    _______,                 _______,    _______,
    _______,    _______,    _______,    _______,    _______,    _______,     _______,   _______,     _______,     _______,   _______,    DEV_RESET,  SLEEP_MODE,              BAT_SHOW,   _______,
    _______,    _______,    _______,    _______,    _______,    _______,     _______,   _______,     _______,     _______,   _______,    _______,                             _______,    _______,
    _______,                _______,    _______,    RGB_TEST,   _______,     BAT_NUM,   _______,     MO(4),       RGB_SPD,   RGB_SPI,    _______,                 _______,    RGB_VAI,    _______,
    _______,    _______,    _______,                                         _______,                             _______,   MO(3),      _______,                 RGB_MOD,    RGB_VAD,    RGB_HUI),

// layer 4
[M_LAYER] = LAYOUT_75_ansi(
    QK_REBOOT,  DB_TOGG,    RF_DFU,     _______,    _______,    _______,     _______,   _______,     _______,     _______,   _______,    DEBOUNCE_D,  DEBOUNCE_I, _______,    _______,    _______,
    _______,    _______,    _______,    _______,    _______,    _______,     _______,   _______,     _______,     _______,   _______,    SLEEP_D,     SLEEP_I,                _______,    _______,
    _______,    _______,    _______,    _______,    _______,    _______,     _______,   _______,     _______,     SIDE_1,    _______,    _______,     SLEEP_NOW,              _______,    _______,
    CAPS_WORD,  _______,    _______,    DEBOUNCE_T, _______,    GAME_MODE,   _______,   _______,     SOCD_TOG,    _______,   _______,    _______,                             _______,    _______,
    _______,                _______,    _______,    _______,    _______,     _______,   NUMLOCK_IND, _______,     SIDE_SPD,  SIDE_SPI,   _______,                 _______,    SIDE_VAI,   _______,
    _______,    _______,    _______,                                         _______,                             _______,   MO(4),      _______,                 SIDE_MOD,   SIDE_VAD,   SIDE_HUI)

};
