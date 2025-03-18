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
#include "redefine.h"
#include "mcu_pwr.h"
#include "version.h"

char            socd_type[4][14] = { "disabled", "cancellation", "exclusion", "nullification" };

/* qmk pre-process record */
bool pre_process_record_kb(uint16_t keycode, keyrecord_t *record) {
    no_act_time      = 0;
    rf_linking_time  = 0;

    // wake up immediately
    if (f_wakeup_prepare) {
        exit_light_sleep(false);
    }

    if (!pre_process_record_user(keycode, record)) {
        return false;
    }

    return true;

}

bool process_record_socd(uint16_t keycode, keyrecord_t *record) {
    if (user_config.socd_mode == 0) { return true; }
    uint8_t socd_array[] = { SOCD_KEYS };
    for (uint8_t idx = 0; idx < sizeof_array(socd_array); ++idx) {
        if ( keycode != socd_array[idx] ) { continue; }

        if (idx % 2 == 0) {
            left_pressed = record->event.pressed;
            idx++;
        } else {
            right_pressed = record->event.pressed;
            idx--;
        }

        if (record->event.pressed) {
            if (right_pressed + left_pressed > 2) {
                unregister_code(socd_array[idx]);
                if (user_config.socd_mode == 3) { return false; }
            }
        } else {
            if (right_pressed + left_pressed > 2) {
                if (user_config.socd_mode >= 2) { register_code(socd_array[idx]); }
            }
        }
        return true;
    }
    return true;
}

/* qmk process record user*/
bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    switch (keycode) {
        case SIDE_VAI:
        case SIDE_VAD:
        case SIDE_HUI:
        case DEBOUNCE_I:
        case DEBOUNCE_D:
        case DEBOUNCE_T:
        case SOCD_TOG:
        case RF_DFU:
            call_update_eeprom_data(&user_update);
            return true;

        case NUMLOCK_IND:
            if (game_mode_enable) { return true; }
            call_update_eeprom_data(&user_update);
            return true;

        case SIDE_MOD:
        case SIDE_SPI:
        case SIDE_SPD:
        case SIDE_1:
        case SLEEP_MODE:
        case SLEEP_I:
        case SLEEP_D:
        case CAPS_WORD:
            if (game_mode_enable) { return false; }
            call_update_eeprom_data(&user_update);
            return true;

        case BAT_SHOW:
        case SLEEP_NOW:
            if (game_mode_enable) { return false; }
            return true;

        case QK_RGB_MATRIX_TOGGLE:
            if (game_mode_enable) { return true; }
            call_update_eeprom_data(&rgb_update);
            return true;

        case QK_RGB_MATRIX_VALUE_UP:
        case QK_RGB_MATRIX_VALUE_DOWN:
        case QK_RGB_MATRIX_SATURATION_UP:
        case QK_RGB_MATRIX_SATURATION_DOWN:
        case QK_RGB_MATRIX_HUE_UP:
        case QK_RGB_MATRIX_HUE_DOWN:
        case QK_RGB_MATRIX_MODE_NEXT:
        case QK_RGB_MATRIX_MODE_PREVIOUS:
            if (game_mode_enable) {
                call_update_eeprom_data(&user_update);
                return true;
            }
            call_update_eeprom_data(&rgb_update);
            return true;

        case QK_RGB_MATRIX_SPEED_UP:
        case QK_RGB_MATRIX_SPEED_DOWN:
            if (game_mode_enable) { return false; }
            call_update_eeprom_data(&rgb_update);
            return true;

        default:
            return true;
    }
}

/* qmk process record */
bool process_record_kb(uint16_t keycode, keyrecord_t *record) {

    if (!process_record_user(keycode, record)) {
        return false;
    }

    if (!process_record_socd(keycode, record)) {
        return false;
    }

    switch (keycode) {
        case RF_DFU:
            if (game_mode_enable) { return false; }
            if (record->event.pressed) {
                f_rf_dfu_press = 1;
            } else {
                if (f_rf_dfu_press) {
                    f_rf_dfu_press = 0;
                    user_config.rf_delay_step = (user_config.rf_delay_step + 1) % 5;
#ifndef NO_DEBUG
                    dprintf("rf_delay: %d\n", user_config.rf_delay_step * 200 + 80);
#endif
                    signal_rgb_led(user_config.rf_delay_step * 2, led_idx.RF_DFU, UINT8_MAX, 3000);
                }
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
                    rf_sw_temp    = keycode - LNK_RF;
                    f_rf_sw_press = 1;
                    break_all_key();
                }
            } else {
                if (f_rf_sw_press) {
                    set_link_mode();
                    uart_send_cmd(CMD_SET_LINK, 10, 20);
                }
                for (uint8_t i = 1; i <= 4; i++) {
                    rgb_matrix_set_color(led_idx.KC_GRV - i, RGB_OFF);
                }
                rgb_matrix_update_pwm_buffers();
            }
            return false;

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
                if (dev_info.sys_sw_state == SYS_SW_MAC) {
                    host_system_send(0x9b);
                }
            } else if (dev_info.sys_sw_state == SYS_SW_MAC) {
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

        case SIDE_VAD:
        case SIDE_VAI:
            if (record->event.pressed) {
                uint8_t dir = keycode % SIDE_VAD;
                side_light_control(dir);
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

        case SIDE_SPD:
        case SIDE_SPI:
            if (record->event.pressed) {
                uint8_t dir = keycode % SIDE_SPD;
                side_speed_control(dir);
            }
            return false;

        case SIDE_1:
            if (record->event.pressed) {
                side_one_control();
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

        case BAT_SHOW:
            if (record->event.pressed) {
                f_bat_hold = !f_bat_hold;
            }
            return false;

        case BAT_NUM:
            f_bat_num_show = record->event.pressed;
            if (!f_bat_num_show) {
                for (uint8_t i = 1; i < 11; i++) {
                    rgb_matrix_set_color(led_idx.KC_GRV - i, RGB_OFF);
                }
                rgb_matrix_update_pwm_buffers();
            }

            return false;

        case RGB_TEST:
            f_rgb_test_press = record->event.pressed;
            return false;

        case NUMLOCK_INS:
            if (record->event.pressed) {
                f_numlock_press = 1;
                if (get_mods() & MOD_MASK_CSA) {
                    tap_code(KC_INS);
                    f_numlock_press = 0;
                }
            } else if (f_numlock_press) {
                f_numlock_press = 0;
                tap_code(KC_INS);
            }
            return false;

        case NUMLOCK_IND:
            if (record->event.pressed) {
                user_config.numlock_state = (user_config.numlock_state + 1) % (3 - game_mode_enable);
            }
            return false;

        case CAPS_WORD:
            f_caps_word_tg = record->event.pressed;
            return false;

        case KC_LGUI:
        case WIN_LOCK:
            if (record->event.pressed) {
                if (get_highest_layer(layer_state) == M_LAYER || keycode == WIN_LOCK) {
                    keymap_config.no_gui = !keymap_config.no_gui;
                    signal_rgb_led(!keymap_config.no_gui * 3, led_idx.KC_LGUI, UINT8_MAX, 3000);
                    return false;
                }
            }
            return true;

        case KC_LSFT:
            if (!record->event.pressed) {
                if ((!user_config.caps_word_enable || game_mode_enable) && is_caps_word_on()) { caps_word_off(); }
            }
            return true;

        case SLEEP_MODE:
            if (record->event.pressed) {
                user_config.sleep_mode = (user_config.sleep_mode + 1) % 3;
                link_timeout           = user_config.sleep_mode == 1 ? (T_MIN * 1) : (T_MIN * 2);
                if (user_config.sleep_mode > 0) {
                    uint8_t temp_sleep = user_config.light_sleep;
                    user_config.light_sleep = user_config.alt_light_sleep;
                    user_config.alt_light_sleep = temp_sleep;
                }

                sleep_show_timer = timer_read32();
            }
            return false;

        case SLEEP_NOW:
            if (USB_ACTIVE) { return false; }
            if (record->event.pressed) {
                wait_ms(100);
            } else {
                if (user_config.sleep_mode == 0) { return true; }
                else {
                    f_goto_sleep     = 1;
                    f_goto_deepsleep = 1;
                    no_act_time      = 100;
                    break_all_key();
                }
            }
            return false;

        case SLEEP_D:
        case SLEEP_I:
            if (user_config.sleep_mode == 0) { return true; }
            if (record->event.pressed) {
                uint8_t dir = keycode % SLEEP_D;
                user_config.light_sleep  = step_helper(dir, user_config.light_sleep);
#ifndef NO_DEBUG
                dprintf("light sleep time:    %dmin\n", user_config.light_sleep);
#endif
            }
            return false;

        case DEBOUNCE_D:
        case DEBOUNCE_I:
            if (record->event.pressed) {
                uint8_t dir = keycode % DEBOUNCE_D;
                user_config.debounce_ms = step_helper(dir, user_config.debounce_ms);
#ifndef NO_DEBUG
                dprintf("debounce:      %dms\n", user_config.debounce_ms);
#endif
            }
            return false;

        case DEBOUNCE_T:
            if (record->event.pressed) {
                debounce_type();
            }
            return false;

        case GAME_MODE:
            if (record->event.pressed) {
                f_gmode_reset_press = 1;
            } else {
                if (f_gmode_reset_press) {
                    f_gmode_reset_press = 0;
                    game_mode_enable = !game_mode_enable;
                    game_mode_tweak();
                }
            }
            return false;

        case SOCD_TOG:
            if (record->event.pressed) {
                user_config.socd_mode = (user_config.socd_mode + 1) % 4;
#ifndef NO_DEBUG
                dprintf("SOCD:    %s(%d)\n", socd_type[user_config.socd_mode], user_config.socd_mode);
#endif
                signal_rgb_led(user_config.socd_mode * 2, led_idx.SOCD_TOG, UINT8_MAX, 3000);

            }
            return false;

        case QK_RGB_MATRIX_VALUE_UP:
            if (record->event.pressed) {
                rgb_matrix_increase_val_noeeprom();
            }
            return false;

        case QK_RGB_MATRIX_VALUE_DOWN:
            if (record->event.pressed) {
                rgb_matrix_decrease_val_noeeprom();
            }
            return false;

        case QK_RGB_MATRIX_MODE_NEXT:
            if (record->event.pressed) {
                if (game_mode_enable) {
                    rgb_matrix_step_game_mode(1);
                    return false;
                }
                rgb_matrix_step_noeeprom();
            }
            return false;

        case QK_RGB_MATRIX_MODE_PREVIOUS:
            if (record->event.pressed) {
                if (game_mode_enable) {
                    rgb_matrix_step_game_mode(0);
                    return false;
                }
                rgb_matrix_step_reverse_noeeprom();
            }
            return false;

        case QK_RGB_MATRIX_HUE_UP:
            if (record->event.pressed) {
                rgb_matrix_increase_hue_noeeprom();
            }
            return false;

        case QK_RGB_MATRIX_HUE_DOWN:
            if (record->event.pressed) {
                rgb_matrix_decrease_hue_noeeprom();
            }
            return false;

        case QK_RGB_MATRIX_SPEED_UP:
            if (record->event.pressed) {
                rgb_matrix_increase_speed_noeeprom();
            }
            return false;

        case QK_RGB_MATRIX_SPEED_DOWN:
            if (record->event.pressed) {
                rgb_matrix_decrease_speed_noeeprom();
            }
            return false;

        case QK_RGB_MATRIX_SATURATION_UP:
            if (record->event.pressed) {
                rgb_matrix_increase_sat_noeeprom();
            }
            return false;

        case QK_RGB_MATRIX_SATURATION_DOWN:
            if (record->event.pressed) {
                rgb_matrix_decrease_sat_noeeprom();
            }
            return false;

        case QK_RGB_MATRIX_TOGGLE:
            if (record->event.pressed) {
                rgb_matrix_toggle_noeeprom();
            }
            return false;

        default:
            return true;
    }
    return true;
}

void post_process_record_kb(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
#ifndef NO_DEBUG
        case DB_TOGG:
            dprintf("Keyboard: %s @ QMK: %s | BUILD: %s (%s)\n", QMK_KEYBOARD, QMK_VERSION, QMK_BUILDDATE, QMK_GIT_HASH);
            break;
#endif
        default:
            break;
    }
}

bool rgb_matrix_indicators_kb(void) {
    if (!rgb_matrix_indicators_user()) {
        return false;
    }

    // low power mode
    power_save();
    // power down unused LEDs
    led_power_handle();
    return true;
}

/* qmk keyboard post init */
void keyboard_post_init_kb(void) {
    gpio_init();
    mcu_timer6_init();
    rf_uart_init();
    reset_led_idx();
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
    interrupt_source_init();
    keyboard_post_init_user();
}


/* qmk housekeeping task */
void housekeeping_task_kb(void) {
    timer_pro();

    uart_receive_pro();

    uart_send_report_repeat();

    dev_sts_sync();

    user_key_press();
//
    led_show();

#ifndef NO_DEBUG
    user_debug();
#endif

    delay_update_eeprom_data();

    if (game_mode_enable) { return; }

    sleep_handle();

    idle_enter_sleep();

}
