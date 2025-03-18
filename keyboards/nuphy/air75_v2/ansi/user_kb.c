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

#include "user_kb.h"
#include "ansi.h"
#include "mcu_pwr.h"
#include "color.h"
#include "settings.h"

user_config_t   user_config;
user_config_t   read_user_config;
led_index_t     led_idx;
DEV_INFO_STRUCT dev_info = {
    .rf_battery = 100,
    .link_mode  = LINK_USB,
    .rf_state   = RF_IDLE,
};
bool f_bat_hold          = 0;
bool game_mode_enable    = 0;
bool f_send_channel      = 0;
bool f_dial_sw_init_ok   = 0;
bool f_bat_num_show      = 0;
bool rgb_power_save      = 0;

uint8_t        rgb_required            = 0;
uint8_t        rf_blink_cnt            = 0;
uint8_t        rf_sw_temp              = 0;
uint8_t        host_mode               = 0;
uint8_t        rgb_interval            = 0;
uint8_t        delay_step_timer        = 0;
uint8_t        long_press_timer        = 0;
uint16_t       side_one_timer          = 0;
uint16_t       rf_linking_time         = 0;
uint16_t       rf_link_show_time       = 0;
uint32_t       no_act_time             = 0;
uint16_t       f_rf_sw_press           = 0;
uint16_t       f_rf_dfu_press          = 0;
uint16_t       f_rgb_test_press        = 0;
uint16_t       f_dev_reset_press       = 0;
uint16_t       f_caps_word_tg          = 0;
uint16_t       f_numlock_press         = 0;
uint16_t       f_gmode_reset_press     = 0;
uint32_t       sys_show_timer          = 0;
uint32_t       sleep_show_timer        = 0;

uint16_t       left_pressed            = 0;
uint16_t       right_pressed           = 0;

host_driver_t *m_host_driver           = 0;

uint16_t       link_timeout            = T_MIN;

uint32_t       eeprom_update_timer     = 0;
bool           user_update             = 0;
bool           rgb_update              = 0;

char           debounce_algo[3][20]    = { "sym_defer_pk", "asym_eager_defer_pk", "sym_eager_pr" };

extern host_driver_t      rf_host_driver;

/**
 * @brief  gpio initial.
 */
void gpio_init(void) {
    /* power on all LEDs */
    pwr_rgb_led_on();
    pwr_side_led_on();
    /* set side led pin output low */
    gpio_set_pin_output_push_pull(DRIVER_SIDE_PIN);
    gpio_write_pin_low(DRIVER_SIDE_PIN);
    /* config RF module pin */
    gpio_set_pin_output_push_pull(NRF_WAKEUP_PIN);
    gpio_write_pin_high(NRF_WAKEUP_PIN);
    gpio_set_pin_input_high(NRF_TEST_PIN);
    /* reset RF module */
    gpio_set_pin_output_push_pull(NRF_RESET_PIN);
    gpio_write_pin_low(NRF_RESET_PIN);
    wait_ms(50);
    gpio_write_pin_high(NRF_RESET_PIN);
    /* config dial switch pin */
    gpio_set_pin_input_high(DEV_MODE_PIN);
    gpio_set_pin_input_high(SYS_MODE_PIN);
}

void set_link_mode(void) {
    f_rf_sw_press = 0;

    dev_info.link_mode   = rf_sw_temp;
    dev_info.rf_channel  = rf_sw_temp;
    dev_info.ble_channel = rf_sw_temp;
}


/**
 * @brief  long press key process.
 */
void user_key_press(void) {
    if (long_press_timer < 10) { return; }
    long_press_timer = 0;

    dial_sw_scan();

    // Open a new RF device
    if (f_rf_sw_press) {
        f_rf_sw_press++;
        if (f_rf_sw_press > MEDIUM_PRESS_DELAY) {
            set_link_mode();
            uint8_t timeout = 5;
            while (timeout--) {
                uart_send_cmd(CMD_NEW_ADV, 0, 1);
                wait_ms(20);
                uart_receive_pro();
                if (f_rf_new_adv_ok) { break; }
            }
        }
    }

    // The device is restored to factory Settings
    if (f_dev_reset_press) {
        f_dev_reset_press++;
        if (f_dev_reset_press > MEDIUM_PRESS_DELAY) {
            f_dev_reset_press = 0;

            if (dev_info.link_mode != LINK_RF_24) {
                dev_info.ble_channel = LINK_BT_1;
                dev_info.rf_channel  = LINK_BT_1;
            }

            if (dev_info.link_mode != LINK_USB) {
                if (dev_info.link_mode != LINK_RF_24) {
                    dev_info.link_mode   = LINK_BT_1;
                }
            }

            uart_send_cmd(CMD_SET_LINK, 10, 10);
            wait_ms(500);
            uart_send_cmd(CMD_CLR_DEVICE, 10, 10);

            void device_reset_show(void);
            void device_reset_init(void);

            eeconfig_init();
            device_reset_show();
            device_reset_init();
            eeconfig_update_rgb_matrix_default();
            dev_info.sys_sw_state = 0;
            dial_sw_fast_scan();
        }
    }

    // Enter the RGB test mode
    if (f_rgb_test_press) {
        f_rgb_test_press++;
        if (f_rgb_test_press > MEDIUM_PRESS_DELAY) {
            f_rgb_test_press = 0;
            rgb_test_show();
        }
    }

    // NumLock Press
    if (f_numlock_press) {
        f_numlock_press++;
        if (f_numlock_press > MICRO_PRESS_DELAY) {
            tap_code(KC_NUM);
            f_numlock_press = 0;
        }
    } 
    // Trigger Game Mode Reset
    if (f_gmode_reset_press) {
        f_gmode_reset_press++;
        if (f_gmode_reset_press > MEDIUM_PRESS_DELAY) {
            game_config_reset(1);
            game_mode_tweak();
            f_gmode_reset_press = 0;
        }
    }
 
    // SnapTap Function
    if (left_pressed)  {   left_pressed++; }
    if (right_pressed) {  right_pressed++; }

    // Toggle Caps Word
    if (f_caps_word_tg) {
        f_caps_word_tg++;
        if (f_caps_word_tg > SMALL_PRESS_DELAY) {
            user_config.caps_word_enable = !user_config.caps_word_enable;
            f_caps_word_tg = 0;
#ifndef NO_DEBUG
            dprintf("caps_word_state: %s\n", user_config.caps_word_enable ? "ON" : "OFF");
#endif
            signal_rgb_led(user_config.caps_word_enable * 3, led_idx.KC_CAPS, UINT8_MAX, CAPS_WORD_IDLE_TIMEOUT);
        }
    }

    // Toggle RF Delay
    if (f_rf_dfu_press) {
        f_rf_dfu_press++;
        if (f_rf_dfu_press > MEDIUM_PRESS_DELAY) {
            f_rf_dfu_press = 0;
#ifndef NO_DEBUG
            dprintf("RF DFU Mode Enabled\n");
#endif
            uart_send_cmd(CMD_RF_DFU, 10, 20);
            signal_rgb_led(9, led_idx.RF_DFU, UINT8_MAX, UINT16_MAX);
        }
    }

}

/**
 * @brief  Release all keys, clear keyboard report.
 */
void break_all_key(void) {
    clear_keyboard();
    void clear_report_buffer_and_queue(void);
    clear_report_buffer_and_queue();
}

/**
 * @brief  switch device link mode.
 * @param mode : link mode
 */
void switch_dev_link(uint8_t mode) {
    if (mode > LINK_USB) { return; }
    no_act_time = 0;
    break_all_key();

    dev_info.link_mode = mode;

    dev_info.rf_state = RF_IDLE;
    f_send_channel    = 1;

    if (mode == LINK_USB) {
        host_mode = HOST_USB_TYPE;
        host_set_driver(m_host_driver);
        rf_link_show_time = 0;
    } else {
        host_mode = HOST_RF_TYPE;
        host_set_driver(&rf_host_driver);
    }
}

/**
 * @brief  read dial values
 */
uint8_t dial_read(void) {
    uint8_t dial_scan = 0;
    gpio_set_pin_input_high(DEV_MODE_PIN);
    gpio_set_pin_input_high(SYS_MODE_PIN);

    if (gpio_read_pin(DEV_MODE_PIN)) { dial_scan |= 0X01; }
    if (gpio_read_pin(SYS_MODE_PIN)) { dial_scan |= 0X02; }

    return dial_scan;
}

/**
 * @brief  set dial values based on input
 * @param dial_scan    : current dial input value
 * @param led_sys_show : show system led
 */
void dial_set(uint8_t dial_scan, bool led_sys_show) {

    if (dial_scan & 0x01) {
        if (dev_info.link_mode != LINK_USB) {
            switch_dev_link(LINK_USB);
        }
    } else {
        if (dev_info.link_mode != dev_info.rf_channel) {
            switch_dev_link(dev_info.rf_channel);
        }
    }

    if (dial_scan & 0x02) {
        if (dev_info.sys_sw_state != SYS_SW_MAC) {
            if (led_sys_show) { sys_show_timer = timer_read32(); }
            default_layer_set(1 << MAC_BASE);
            dev_info.sys_sw_state = SYS_SW_MAC;
            keymap_config.nkro    = 0;
            reset_led_idx();
        }
    } else {
        if (dev_info.sys_sw_state != SYS_SW_WIN) {
            if (led_sys_show) { sys_show_timer = timer_read32(); }
            default_layer_set(1 << WIN_BASE);
            dev_info.sys_sw_state = SYS_SW_WIN;
            keymap_config.nkro    = 1;
            reset_led_idx();
        }
    }
}

/**
 * @brief  scan dial switch.
 */
void dial_sw_scan(void) {
    uint8_t         dial_scan       = 0;
    static uint8_t  dial_save       = 0xf0;
    static uint8_t  debounce        = 0;
    dial_scan       = dial_read();

    if (dial_save != dial_scan) {
        break_all_key();

        no_act_time       = 0;
        rf_linking_time   = 0;
        f_wakeup_prepare  = 0;

        dial_save         = dial_scan;
        debounce          = 10;
        f_dial_sw_init_ok = 0;
        return;
    } else if (debounce) {
        debounce--;
        return;
    }

    dial_set(dial_scan, true);

    if (f_dial_sw_init_ok == 0) {
        f_dial_sw_init_ok = 1;

        if (dev_info.link_mode != LINK_USB) {
            host_set_driver(&rf_host_driver);
        }
    }
}


/**
 * @brief  power on scan dial switch.
 */
void dial_sw_fast_scan(void) {
    uint8_t         dial_scan   = 0;
    uint8_t         dial_check  = 0xf0;
    uint8_t         debounce    = 0;

    // Debounce to get a stable state
    for (debounce = 0; debounce < 10; debounce++) {
        dial_scan       = dial_read();
        if (dial_check != dial_scan) {
            dial_check = dial_scan;
            debounce       = 0;
        }
        wait_ms(1);
    }

    dial_set(dial_scan, false);
}

/**
 * @brief  timer process.
 */
void timer_pro(void) {
    static uint32_t interval_timer = 0;
    static uint16_t missing_time   = 0;
    static uint16_t adjust_time    = 0;
    static bool     f_first        = true;

    if (f_first) {
        f_first        = false;
        interval_timer = timer_read32();
        m_host_driver  = host_get_driver();
    }

    // step 10ms
    if (timer_elapsed32(interval_timer) < 10) { return; }

    if (!game_mode_enable && no_act_time >= 1000 ) {
        missing_time += (timer_elapsed32(interval_timer) - 10);
        if (missing_time >= 100 && no_act_time % 3000 != 0) {
            adjust_time  = missing_time / 10;
            missing_time = 0;
        }
    }

    interval_timer = timer_read32();

    if (rf_link_show_time < RF_LINK_SHOW_TIME) { rf_link_show_time++; }

    if (no_act_time < UINT32_MAX) { no_act_time += adjust_time + 1; }

    if (rf_linking_time < UINT16_MAX) { rf_linking_time += adjust_time + 1; }

    if (rgb_interval < UINT8_MAX) { rgb_interval++; }

    if (delay_step_timer < UINT8_MAX) { delay_step_timer++; }

    if (long_press_timer < UINT8_MAX) { long_press_timer++; }

    if (side_one_timer < UINT16_MAX) { side_one_timer += adjust_time + 1; }

    adjust_time = 0;
}

/**
 * @brief  load eeprom data.
 */
void load_eeprom_data(void) {
    eeconfig_read_kb_datablock(&user_config);
    if (user_config.init_layer < 100) { user_config_reset(); }
}

void call_update_eeprom_data(bool* eeprom_update_init) {
    *eeprom_update_init = 1;
    eeprom_update_timer = 0;
}

/**
 * @brief User config update to eeprom with delay
 */
void delay_update_eeprom_data(void) {
    if (eeprom_update_timer == 0) {
        if (user_update || rgb_update) { eeprom_update_timer = timer_read32(); }
        return;
    }
    if (timer_elapsed32(eeprom_update_timer) < (1000 * 30)) { return; }
    if (user_update) {
        if (game_mode_enable) {
            eeconfig_read_kb_datablock(&read_user_config);
            read_user_config.game_rgb_val       = rgb_matrix_config.hsv.v;
            read_user_config.game_rgb_hue       = rgb_matrix_config.hsv.h;
            read_user_config.game_rgb_sat       = rgb_matrix_config.hsv.s;
            read_user_config.game_rgb_mod       = rgb_matrix_config.mode;
            read_user_config.game_side_colour   = user_config.ee_side_colour;
            read_user_config.game_side_light    = user_config.ee_side_light;
            read_user_config.game_debounce_ms   = user_config.debounce_ms;
            read_user_config.game_debounce_type = user_config.debounce_type;

            eeconfig_update_kb_datablock(&read_user_config);
        } else {
            eeconfig_update_kb_datablock(&user_config);
        }

        user_update         = 0;
#ifndef NO_DEBUG
        dprint("Updating EEPROM: user_config\n");
#endif
    }
    if (rgb_update) {
        eeconfig_update_rgb_matrix();
        rgb_update          = 0;
#ifndef NO_DEBUG
        dprint("Updating EEPROM:  rgb_config\n");
#endif
    }
    eeprom_update_timer = 0;
}

void game_mode_tweak(void)
{
    if (game_mode_enable) {
        if (eeprom_update_timer != 0) {
            eeprom_update_timer = 0;
            if (user_update) {
                eeconfig_update_kb_datablock(&user_config);
                user_update = 0;
#ifndef NO_DEBUG
                dprint("Updating EEPROM: user_config\n");
#endif
            }
            if (rgb_update) {
                eeconfig_update_rgb_matrix();
                rgb_update          = 0;
#ifndef NO_DEBUG
                dprint("Updating EEPROM:  rgb_config\n");
#endif
            }
        }

        pwr_rgb_led_on();
        rgb_matrix_mode_noeeprom(user_config.game_rgb_mod);
        rgb_matrix_config.hsv.v    = user_config.game_rgb_val;
        rgb_matrix_config.hsv.h    = user_config.game_rgb_hue;
        rgb_matrix_config.hsv.s    = user_config.game_rgb_sat;
        user_config.ee_side_mode   = 2;
        user_config.ee_side_rgb    = 0;
        user_config.ee_side_colour = user_config.game_side_colour;
        user_config.ee_side_light  = user_config.game_side_light;
        user_config.debounce_ms    = user_config.game_debounce_ms;
        user_config.debounce_type  = user_config.game_debounce_type;
        if (user_config.numlock_state != 0) { user_config.numlock_state = 1; }
    } else {
        rgb_matrix_reload_from_eeprom();
        eeconfig_read_kb_datablock(&user_config);
    }

#ifndef NO_DEBUG
    dprintf("debounce:      %dms\n", user_config.debounce_ms);
    dprintf("debounce type: %s\n", debounce_algo[user_config.debounce_type]);
#endif
    pwr_rgb_led_on();
    signal_rgb_led(game_mode_enable * 3, led_idx.KC_G, UINT8_MAX, 2000);
}

void reset_led_idx(void) {
    led_idx.KC_CAPS  = get_led_idx(KC_CAPS);
    led_idx.KC_LGUI  = get_led_idx(KC_LGUI);

    uint16_t numlock_keys[4] = { KC_NUM, NUMLOCK_INS, KC_INS, KC_MINS };
    for (uint8_t i = 0; i < sizeof_array(numlock_keys); i++) {
        led_idx.KC_NUM  = get_led_idx(numlock_keys[i]);
        if (led_idx.KC_NUM != UINT8_MAX) { break; }
    }

    led_idx.KC_D     = get_led_idx(DEBOUNCE_T);
    led_idx.KC_G     = get_led_idx(GAME_MODE);
    led_idx.KC_F1    = get_led_idx(KC_F1);
    led_idx.RF_DFU   = get_led_idx(RF_DFU);
    led_idx.SOCD_TOG = get_led_idx(SOCD_TOG);
    led_idx.KC_F12   = get_led_idx(KC_F12);
    led_idx.KC_GRV   = get_led_idx(KC_GRV);
}

uint8_t get_led_idx(uint16_t keycode) {
    uint8_t current_default = dev_info.sys_sw_state == SYS_SW_WIN ? WIN_BASE : MAC_BASE;
    uint8_t check_layers[3] = { current_default, (current_default + 1), M_LAYER };
    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
        for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
            for (uint8_t layer = 0; layer < 3; ++layer) {
                if (keycode_at_keymap_location(check_layers[layer], row, col) == keycode) {
                    return g_led_config.matrix_co[row][col];
                }
            }
        }
    }
    return UINT8_MAX;
}

uint8_t step_helper(uint8_t dir, uint8_t value) {
    uint8_t step, my_color, end_led;
    if (value < 11 - dir) {
        step = 1;
    } else if (value < 31 - dir) {
        step = 2;
    } else if (value < 76 - dir) {
        step = 5;
    } else {
        step = 25;
    }

    if (dir) {
        if (value < 100) { value += step; }
    } else {
        if (value > 1)   { value -= step; }
    }

    if (value <= 10) {
        my_color = 3;
        end_led  = value - 1;
    } else if (value <= 30) {
        my_color = 2;
        end_led  = (value - 10) / 2 - 1;
    } else if (value <= 75) {
        my_color = 0;
        end_led  = (value - 30) / 5 - 1;
    } else {
        my_color = 7;
        end_led  = 9;    
    }

    signal_rgb_led(my_color, led_idx.KC_F1, led_idx.KC_F1 + end_led, 3000);
    return value;
}

void debounce_type(void) {
    user_config.debounce_type = (user_config.debounce_type + 1) % 3;

#ifndef NO_DEBUG
    dprintf("debounce type: %s\n", debounce_algo[user_config.debounce_type]);
#endif
    signal_rgb_led(user_config.debounce_type == 1 ? 3 : user_config.debounce_type, led_idx.KC_D, UINT8_MAX, 3000);
}


#ifndef NO_DEBUG
void user_debug(void) {
    static uint32_t last_print = 0;
    if (no_act_time == 0 || no_act_time == last_print || no_act_time % 3000 != 0) { return; }
    if (!USB_ACTIVE && debug_enable) {
        debug_enable = false;
        print("DEBUG: disabled.\n");
    }
    last_print = no_act_time;
    dprintf("no_act_time: %lds\n", no_act_time / 100);
}
#endif

/**
 * @brief User config to default setting.
 */
void user_config_reset(void) {
    /* first power on, set rgb matrix brightness at middle level*/

    user_config.init_layer              = 100 + DEFAULT_LAYER;
    user_config.ee_side_mode            = 0;
    user_config.ee_side_light           = 1;
    user_config.ee_side_speed           = 2;
    user_config.ee_side_rgb             = 1;
    user_config.ee_side_colour          = 0;
    user_config.ee_side_one             = 0;
    user_config.debounce_ms             = DEBOUNCE;
    user_config.debounce_type           = 1;
    user_config.sleep_mode              = 1;
    user_config.light_sleep             = 2;
    user_config.alt_light_sleep         = 6;
    user_config.caps_word_enable        = 1;
    user_config.numlock_state           = 1;
    user_config.socd_mode               = 0;
    user_config.rf_delay_step           = 2;
    keymap_config.no_gui                = 0;
    game_config_reset(0);
    user_config_override();
    eeconfig_update_kb_datablock(&user_config);
}

void game_config_reset(uint8_t save_to_eeprom) {
    if (save_to_eeprom) { eeconfig_read_kb_datablock(&user_config); }
    user_config.game_side_colour       = SIDE_MATRIX_GAME_MODE;
    user_config.game_side_light        = 2;
    user_config.game_rgb_mod           = RGB_MATRIX_GAME_MODE;
    user_config.game_rgb_val           = RGB_MATRIX_DEFAULT_VAL;
    user_config.game_rgb_hue           = RGB_MATRIX_DEFAULT_HUE;
    user_config.game_rgb_sat           = RGB_MATRIX_DEFAULT_SAT;
    user_config.game_debounce_ms       = DEBOUNCE;
    user_config.game_debounce_type     = 1;
    game_config_override();
    if (save_to_eeprom) { eeconfig_update_kb_datablock(&user_config); }
}

void power_save(void) {
    static uint16_t power_check_timer      = 0;
    if (timer_elapsed32(power_check_timer) < 5000) { return; }

    if (dev_info.rf_battery > low_bat_level && rgb_power_save == 1) {
        if (game_mode_enable) {
            rgb_matrix_config.hsv.v = user_config.game_rgb_val;
            user_config.ee_side_light = user_config.game_side_light;
        } else {
            rgb_matrix_reload_from_eeprom();
            eeconfig_read_kb_datablock(&user_config);
        }
        rgb_power_save = 0;
    } else if ((rgb_matrix_config.hsv.v > 0 || user_config.ee_side_light > 1) && dev_info.rf_battery < low_bat_level) {
        rgb_matrix_config.hsv.v   = 0;
        user_config.ee_side_light = 1;
        rgb_power_save            = 1;
    }
}

/**
 * @brief Handle LED power
 * @note Turn off LEDs if not used to save some power. This is ported
 *       from older Nuphy leaks.
 */
void led_power_handle(void) {
    static uint8_t led_debounce = 4;

    uint16_t led_interval = rgb_required == 1 ? 10: 50;

    if (rgb_interval < led_interval || f_wakeup_prepare || game_mode_enable) {
        return;
    }


    if (rgb_matrix_is_enabled() && (rgb_matrix_get_val() != 0 || rgb_required > 0)) {
        pwr_rgb_led_on();
        rgb_required = 0;
    } else if (rgb_interval > 50) {
        pwr_rgb_led_off();
    }

    if (!is_side_ws2812_off()) {
        pwr_side_led_on();
        led_debounce = 4;
    } else if (led_debounce--) {
        rgb_interval = 0;
        return;
    } else if (rgb_interval > 50) {
        pwr_side_led_off();
    }

    rgb_interval = 0;
}
