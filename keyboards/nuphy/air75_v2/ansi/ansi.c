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

/*
typedef void    (*rgb_func_pointer)(void);
void            rgb_process_record_helper(const rgb_func_pointer rgb_func_noeeprom);
typedef void    (*side_func_pointer)(uint8_t adjust);
void            side_process_record_helper(const side_func_pointer side_func_noeeprom, uint8_t adjust);
*/

/* qmk pre-process record */

bool pre_process_record_kb(uint16_t keycode, keyrecord_t *record) {
    no_act_time      = 0;
    rf_linking_time  = 0;

    // wake up immediately
    if (f_wakeup_prepare) {
        exit_light_sleep(false);
        f_wakeup_prepare = 0;
    }

    if (!pre_process_record_user(keycode, record)) {
        return false;
    }

    return true;

}


/* qmk process record */
bool process_record_kb(uint16_t keycode, keyrecord_t *record) {

    if (!process_record_user(keycode, record)) {
        return false;
    }

    switch (keycode) {
        case SIDE_VAI:
        case SIDE_VAD:
        case SIDE_HUI:
        case NUMLOCK_IND:
            if (game_mode_enable) { break; }
            call_update_eeprom_data(&user_update);
            break;

        case SIDE_MOD:
        case SIDE_SPI:
        case SIDE_SPD:
        case SIDE_1:
        case SLEEP_MODE:
        case CAPS_WORD:
            if (game_mode_enable) { return false; }
            call_update_eeprom_data(&user_update);
            break;

        case BAT_SHOW:
        case SLEEP_NOW:
            if (game_mode_enable) { return false; }
            break;

        case RGB_VAI:
        case RGB_VAD:
            if (game_mode_enable) { break; }
            call_update_eeprom_data(&rgb_update);
            break;

        case RGB_MOD:
        case RGB_RMOD:
        case RGB_HUI:
        case RGB_HUD:
        case RGB_SPI:
        case RGB_SPD:
        case RGB_SAI:
        case RGB_SAD:
        case RGB_M_P:
        case RGB_TOG:
            if (game_mode_enable) { return false; }
            call_update_eeprom_data(&rgb_update);
            break;
        }

    switch (keycode) {
        case RF_DFU:
            if (record->event.pressed) {
                if (dev_info.link_mode != LINK_USB) { return false; }
                uart_send_cmd(CMD_RF_DFU, 10, 20);
            }
            return false;

        case LNK_USB:
            if (record->event.pressed) {
                break_all_key();
            } else {
                dev_info.link_mode = LINK_USB;
                uart_send_cmd(CMD_SET_LINK, 10, 10);
            }
            return false;

        case LNK_RF ... LNK_BLE3:
            if (record->event.pressed) {
                if (dev_info.link_mode != LINK_USB) {
                    rf_sw_temp    = keycode - 2;
                    f_rf_sw_press = 1;
                    break_all_key();
                }
            } else {
                if (f_rf_sw_press) {
                    if (rf_sw_press_delay < MEDIUM_PRESS_DELAY) {
                        set_link_mode();
                        uart_send_cmd(CMD_SET_LINK, 10, 20);
                    }
                }
                rf_sw_press_delay = 0;
            }
            return false;

        case DEV_RESET:
            if (record->event.pressed) {
                f_dev_reset_press = 1;
                break_all_key();
            } else {
                f_dev_reset_press = 0;
            }
            return false;

        case GAME_MODE:
            if (record->event.pressed) {
                game_mode_enable = !game_mode_enable;
                game_mode_tweak();
            }
            return false;

        case CAPS_WORD:
            f_caps_word_tg = record->event.pressed;
            return false;

        case WIN_LOCK:
            if (record->event.pressed) {
                keymap_config.no_gui = !keymap_config.no_gui;
                signal_rgb_led(!keymap_config.no_gui, WIN_LED, WIN_LED, 3000);
            }
            return false;

        case KC_LSFT:
            if (!record->event.pressed) {
                if ((!user_config.caps_word_enable || game_mode_enable) && is_caps_word_on()) { caps_word_off(); }
            }
            return true;

        case MAC_VOICE:
            if (record->event.pressed) {
                if (dev_info.sys_sw_state == SYS_SW_MAC) {
                    host_consumer_send(0xcf);
                } else {
                    tap_code(KC_F5);
                }
            } else if (dev_info.sys_sw_state == SYS_SW_MAC) {
                host_consumer_send(0);
            }
            return false;

        case MAC_DND:
            if (record->event.pressed) {
                host_system_send(0x9b);
            } else {
                host_system_send(0);
            }
            return false;

        case TASK:
            if (record->event.pressed) {
                if (dev_info.sys_sw_state == SYS_SW_MAC) {
                    tap_code(KC_MCTL);
                } else {
                    tap_code(KC_CALC);
                }
            }
            return false;

        case SEARCH:
            if (record->event.pressed) {
                if (dev_info.sys_sw_state == SYS_SW_MAC) {
                    register_code(KC_LGUI);
                    register_code(KC_SPACE);
                    wait_ms(TAP_CODE_DELAY);
                    unregister_code(KC_LGUI);
                    unregister_code(KC_SPACE);
                } else {
                    register_code(KC_LCTL);
                    register_code(KC_F);
                    wait_ms(TAP_CODE_DELAY);
                    unregister_code(KC_F);
                    unregister_code(KC_LCTL);
                }
            }
            return false;

        case PRT_SCR:
            if (record->event.pressed) {
                if (dev_info.sys_sw_state == SYS_SW_MAC) {
                    register_code(KC_LGUI);
                    register_code(KC_LSFT);
                    register_code(KC_3);
                    wait_ms(TAP_CODE_DELAY);
                    unregister_code(KC_3);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LGUI);
                } else {
                    tap_code(KC_PSCR);
                }
            }
            return false;

        case PRT_AREA:
            if (record->event.pressed) {
                if (dev_info.sys_sw_state == SYS_SW_MAC) {
                    register_code(KC_LGUI);
                    register_code(KC_LSFT);
                    register_code(KC_4);
                    wait_ms(TAP_CODE_DELAY);
                    unregister_code(KC_4);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LGUI);
                }
                else {
                    tap_code(KC_PSCR);
                }
            }
            return false;

        case SIDE_VAI:
            if (record->event.pressed) {
                side_light_control(1);
            }
            return false;

        case SIDE_VAD:
            if (record->event.pressed) {
                side_light_control(0);
            }
            return false;

        case SIDE_MOD:
            if (record->event.pressed) {
                side_mode_control(1);
            }
            return false;

        case SIDE_HUI:
            if (record->event.pressed) {
                side_colour_control(1);
            }
            return false;

        case SIDE_SPI:
            if (record->event.pressed) {
                side_speed_control(1);
            }
            return false;

        case SIDE_SPD:
            if (record->event.pressed) {
                side_speed_control(0);
            }
            return false;

        case SIDE_1:
            if (record->event.pressed) {
                side_one_control(1);
            }
            return false;

        case RGB_VAI:
            if (record->event.pressed) {
                rgb_matrix_increase_val_noeeprom();
            }
            return false;

        case RGB_VAD:
            if (record->event.pressed) {
                rgb_matrix_decrease_val_noeeprom();
            }
            return false;

        case RGB_MOD:
            if (record->event.pressed) {
                rgb_matrix_step_noeeprom();
            }
            return false;

        case RGB_RMOD:
            if (record->event.pressed) {
                rgb_matrix_step_reverse_noeeprom();
            }
            return false;

        case RGB_HUI:
            if (record->event.pressed) {
                rgb_matrix_increase_hue_noeeprom();
            }
            return false;

        case RGB_HUD:
            if (record->event.pressed) {
                rgb_matrix_decrease_hue_noeeprom();
            }
            return false;

        case RGB_SPI:
            if (record->event.pressed) {
                rgb_matrix_increase_speed_noeeprom();
            }
            return false;

        case RGB_SPD:
            if (record->event.pressed) {
                rgb_matrix_decrease_speed_noeeprom();
            }
            return false;

        case RGB_SAI:
            if (record->event.pressed) {
                rgb_matrix_increase_sat_noeeprom();
            }
            return false;

       case RGB_SAD:
            if (record->event.pressed) {
                rgb_matrix_decrease_sat_noeeprom();
            }
            return false;

       case RGB_M_P:
            if (record->event.pressed) {
                rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            }
            return false;

       case RGB_TOG:
            if (record->event.pressed) {
                rgb_matrix_toggle_noeeprom();
            }
            return false;

        case SLEEP_MODE:
            if (record->event.pressed) {
                user_config.sleep_mode++;
                if (user_config.sleep_mode < 3) {
                    link_timeout     = (NO_ACT_TIME_MINUTE * user_config.sleep_mode);
                    sleep_time_delay = (NO_ACT_TIME_MINUTE * (4 * user_config.sleep_mode - 2));
                } else { user_config.sleep_mode = 0; }
                sleep_show_timer = timer_read32();
            }
            return false;

        case BAT_SHOW:
            if (record->event.pressed) {
                f_bat_hold = !f_bat_hold;
            }
            return false;

        case BAT_NUM:
            f_bat_num_show = record->event.pressed;
            return false;

        case RGB_TEST:
            f_rgb_test_press = record->event.pressed;
            return false;

        case NUMLOCK_INS:
            if (record->event.pressed) {
                if (get_mods() & MOD_MASK_CSA) {
                    tap_code(KC_INS);
                    f_numlock_press = 0;
                } else { f_numlock_press = 1; }
            } else if (f_numlock_press) {
                f_numlock_press = 0;
                tap_code(KC_INS);
            }
            return false;

        case NUMLOCK_IND:
            if (record->event.pressed) {
                if (user_config.numlock_state < 2 - game_mode_enable) { user_config.numlock_state++; }
                else { user_config.numlock_state = 0; }
            }
            return false;

        case SLEEP_NOW:
            if (record->event.pressed) {
                wait_ms(100);
            } else {
                if (user_config.sleep_mode == 0) { return true; }
                else {
                    f_goto_sleep = 1;
                    f_goto_deepsleep  = 1;
                }
            }

            return true;

        default:
            return true;
    }
    return true;
}

bool rgb_matrix_indicators_kb(void) {
    if (!rgb_matrix_indicators_user()) {
        return false;
    }

    if(f_bat_num_show) {
        bat_num_led();
    }

    // power down unused LEDs
    led_power_handle();
    return true;
}

/* qmk keyboard post init */
void keyboard_post_init_kb(void) {
    gpio_init();
    rf_uart_init();
    wait_ms(500);
    rf_device_init();

    break_all_key();
    load_eeprom_data();
    dial_sw_fast_scan();
#ifndef NO_DEBUG
    debug_enable   = false;
    // debug_matrix   = true;
    // debug_keyboard = true;
    // debug_mouse    = true;
#endif
    keyboard_post_init_user();
}

/*
void rgb_process_record_helper(const rgb_func_pointer rgb_func_noeeprom) {
    rgb_func_noeeprom();
    eeprom_update_timer = 0;
    rgb_update = 1;
}

void side_process_record_helper(const side_func_pointer side_func_noeeprom, uint8_t adjust) {
    side_func_noeeprom(adjust);
    eeprom_update_timer = 0;
    user_update = 1;
}
*/


/* qmk housekeeping task */
void housekeeping_task_kb(void) {
    timer_pro();

    uart_receive_pro();

    uart_send_report_repeat();

    dev_sts_sync();

    custom_key_press();
//
    led_show();

#ifndef NO_DEBUG
    user_debug();
#endif

    if (game_mode_enable) { return; }

    delay_update_eeprom_data();

    sleep_handle();

}
