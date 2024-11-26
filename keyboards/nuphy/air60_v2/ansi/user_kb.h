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

#include "quantum.h"
#include "usb_main.h"
#include "layer_names.h"
#include "keymap_introspection.h"

typedef enum {
    RX_Idle,
    RX_Receiving,
    RX_Done,
    RX_Fail,
    RX_OV_ERR,
    RX_SUM_ERR,
    RX_CMD_ERR,
    RX_DATA_ERR,
    RX_DATA_OV,
    RX_FORMAT_ERR,

    TX_OK = 0XE0,
    TX_DONE,
    TX_BUSY,
    TX_TIMEOUT,
    TX_DATA_ERR,

} TYPE_RX_STATE;


#define RF_IDLE                   0
#define RF_PAIRING                1
#define RF_LINKING                2
#define RF_CONNECT                3
#define RF_DISCONNECT             4
#define RF_SLEEP                  5
#define RF_SNIF                   6
#define RF_INVALID                0XFE
#define RF_ERR_STATE              0XFF

#define CMD_POWER_UP              0XF0
#define CMD_SLEEP                 0XF1
#define CMD_HAND                  0XF2
#define CMD_SNIF                  0XF3
#define CMD_24G_SUSPEND           0XF4
#define CMD_IDLE_EXIT             0XFE

#define CMD_RPT_MS                0XE0
#define CMD_RPT_BYTE_KB           0XE1
#define CMD_RPT_BIT_KB            0XE2
#define CMD_RPT_CONSUME           0XE3
#define CMD_RPT_SYS               0XE4

#define CMD_SET_LINK              0XC0
#define CMD_SET_CONFIG            0XC1
#define CMD_GET_CONFIG            0XC2
#define CMD_SET_NAME              0XC3
#define CMD_GET_NAME              0XC4
#define CMD_CLR_DEVICE            0XC5
#define CMD_NEW_ADV               0XC7
#define CMD_RF_STS_SYSC           0XC9
#define CMD_SET_24G_NAME          0XCA
#define CMD_GO_TEST               0XCF
#define CMD_RF_DFU                0XB1
#define CMD_NULL                  0X00

#define CMD_WRITE_DATA            0X80
#define CMD_READ_DATA             0X81

#define LINK_RF_24                0
#define LINK_BT_1                 1
#define LINK_BT_2                 2
#define LINK_BT_3                 3
#define LINK_USB                  4

#define UART_HEAD                 0x5A
#define FUNC_VALID_LEN            32
#define UART_MAX_LEN              64

#define SYS_SW_WIN                0xa1
#define SYS_SW_MAC                0xa2

#define RF_LINK_SHOW_TIME         200

#define HOST_USB_TYPE             0
#define HOST_BLE_TYPE             1
#define HOST_RF_TYPE              2

#define RF_POWER_DOWN_DELAY       (30)

#define MICRO_PRESS_DELAY         5
#define SMALL_PRESS_DELAY         10
#define MEDIUM_PRESS_DELAY        30
#define LONG_PRESS_DELAY          50

#define T_MIN                     (60 * 100)
#define DEEP_SLEEP_TIME           4

#define RGB_MATRIX_GAME_MODE      RGB_MATRIX_GRADIENT_LEFT_RIGHT
#define SIDE_MATRIX_GAME_MODE     4
#define SOCD_KEYS                 KC_A, KC_D, KC_LEFT, KC_RIGHT

/*
#define    CAPS_LED               28
#define    LSHIFT_LED             41
#define    WIN_LED                (dev_info.sys_sw_state == SYS_SW_WIN ? 56 : 57)
#define    NUMLOCK_LED            47
#define    G_LED                  33
*/

#ifndef DEBOUNCE
#    define DEBOUNCE 5
#endif

#define sizeof_array(x)           (sizeof(x)/sizeof(*(x)))
#define USB_ACTIVE                ((dev_info.link_mode == LINK_USB && USB_DRIVER.state != USB_SUSPENDED) || (dev_info.link_mode != LINK_USB && dev_info.rf_charge == 0x03))

typedef struct
{
    uint8_t RXDState;
    uint8_t RXDLen;
    uint8_t RXDOverTime;
    uint8_t TXDLenBack;
    uint8_t TXDOffset;
    uint8_t RXCmd;
    uint8_t TXDBuf[UART_MAX_LEN];
    uint8_t RXDBuf[UART_MAX_LEN];
} USART_MGR_STRUCT;

typedef struct
{
    uint8_t link_mode;
    uint8_t rf_channel;
    uint8_t ble_channel;
    uint8_t rf_state;
    uint8_t rf_charge;
    uint8_t rf_led;
    uint8_t rf_battery;
    uint8_t sys_sw_state;
} DEV_INFO_STRUCT;

typedef struct
{
    uint8_t init_layer;
    uint8_t ee_side_mode;
    uint8_t ee_side_light;
    uint8_t ee_side_speed;
    uint8_t ee_side_rgb;
    uint8_t ee_side_colour;
    uint8_t ee_side_one;
    uint8_t sleep_mode;
    uint8_t caps_word_enable;
    uint8_t numlock_state;
    uint8_t debounce_ms;
    uint8_t debounce_type;
    uint8_t light_sleep;
    uint8_t alt_light_sleep;
    uint8_t game_side_colour;
    uint8_t game_side_light;
    uint8_t game_rgb_mod;
    uint8_t game_rgb_val;
    uint8_t game_rgb_hue;
    uint8_t game_rgb_sat;
    uint8_t game_debounce_ms;
    uint8_t game_debounce_type;
    uint8_t socd_mode;
    uint8_t retain1;
    uint8_t retain2;
} user_config_t;

_Static_assert(sizeof(user_config_t) == EECONFIG_KB_DATA_SIZE, "Mismatch in user EECONFIG stored data");


typedef struct
{
    uint8_t KC_CAPS;
    uint8_t KC_LGUI;
    uint8_t KC_NUM;
    uint8_t KC_D;
    uint8_t KC_G;
    uint8_t KC_F1;
    uint8_t RF_DFU;
    uint8_t KC_TAB;
    uint8_t SOCD_TOG;
} led_index_t;

extern led_index_t        led_idx;
extern DEV_INFO_STRUCT    dev_info;
extern user_config_t      user_config;

extern uint8_t            rf_blink_cnt;
extern uint16_t           rf_link_show_time;
extern uint8_t            rf_disconnect_delay;

extern bool               f_bat_hold;
extern bool               game_mode_enable;
extern uint32_t           sys_show_timer;
extern uint32_t           sleep_show_timer;
extern uint16_t           f_rf_sw_press;
extern uint16_t           f_dev_reset_press;
extern bool               f_bat_num_show;
extern uint16_t           f_rgb_test_press;
extern uint16_t           f_caps_word_tg;
extern uint16_t           f_numlock_press;
extern uint16_t           f_gmode_reset_press;

extern uint8_t            rf_sw_temp;
extern uint16_t           rf_sw_press_delay;
extern uint16_t           rf_linking_time;
extern bool               f_rf_new_adv_ok;

extern uint32_t           no_act_time;
extern uint16_t           sleep_time_delay;
extern uint32_t           deep_sleep_delay;
extern uint16_t           link_timeout;
extern bool               f_rf_sleep;
extern bool               f_wakeup_prepare;
extern bool               f_goto_sleep;
extern bool               f_goto_deepsleep;


extern uint32_t           eeprom_update_timer;
extern uint32_t           dequeue_delay;
extern bool               rgb_update;
extern bool               user_update;
extern uint8_t            rgb_required;
extern uint16_t           left_pressed;
extern uint16_t           right_pressed;

extern bool               is_side_rgb_off(void);
extern void               user_config_override(void);
extern void               game_config_override(void);

void    dev_sts_sync(void);
void    rf_uart_init(void);
void    rf_device_init(void);
void    uart_send_report_repeat(void);
void    uart_receive_pro(void);
void    uart_send_report(uint8_t report_type, uint8_t *report_buf, uint8_t report_size);
void    signal_sleep(uint8_t r, uint8_t g, uint8_t b);
void    side_speed_control(uint8_t dir);
void    side_light_control(uint8_t dir);
void    side_colour_control(uint8_t dir);
void    side_mode_control(uint8_t dir);
void    side_one_control(uint8_t dir);
void    led_show(void);
void    sleep_handle(void);
void    bat_num_led(void);
void    rgb_test_show(void);
void    gpio_init(void);
void    custom_key_press(void);
void    break_all_key(void);
void    switch_dev_link(uint8_t mode);
void    dial_sw_scan(void);
void    dial_sw_fast_scan(void);
void    timer_pro(void);
void    load_eeprom_data(void);
void    delay_update_eeprom_data(void);
void    user_config_reset(void);
void    led_power_handle(void);
void    set_link_mode(void);
void    matrix_io_delay(void);
void    game_mode_tweak(void);
void    user_debug(void);
void    call_update_eeprom_data(bool* eeprom_update_init);
void    signal_rgb_led(uint8_t color, uint8_t bin_type, uint8_t start_led, uint8_t end_led, uint16_t show_time);
void    reset_led_idx(void);
void    game_config_reset(uint8_t save_to_eeprom);
void    debounce_type(void);
uint8_t step_helper(uint8_t dir, uint8_t value);
uint8_t get_led_idx(uint16_t keycode);
uint8_t get_array_idx(uint8_t *array, uint8_t size, uint16_t elm);
void    rgb_matrix_step_game_mode(uint8_t step);
uint8_t uart_send_cmd(uint8_t cmd, uint8_t ack_cnt, uint8_t delayms);
