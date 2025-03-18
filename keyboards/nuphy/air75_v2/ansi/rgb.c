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
#include "rgb_table.h"
#include "ws2812_driver.h"
#include "mcu_pwr.h"

/* side rgb mode */
enum {
    SIDE_WAVE = 0,
    SIDE_MIX,
    SIDE_STATIC,
    SIDE_BREATH,
    SIDE_OFF,
};

uint8_t  side_play_point      = 0;
uint8_t  rgb_color            = 0;
uint8_t  rgb_start_led        = 0;
uint8_t  rgb_end_led          = 0;
uint32_t rgb_show_time        = 0;
uint32_t rgb_indicator_timer  = 0;
uint8_t  low_bat_level        = 20;

uint8_t  sys_light            = 3;

hsv_t hsv = { .h = 0, .s = 255, .v = 255};
rgb_t current_rgb = {.r = 0x00, .g = 0x00, .b = 0x00};

const uint8_t side_speed_table[5][5] = {
    [SIDE_WAVE]   = {12, 18, 23, 28, 38},
    [SIDE_MIX]    = {14, 19, 25, 32, 40},
    [SIDE_STATIC] = {50, 50, 50, 50, 50},
    [SIDE_BREATH] = {12, 18, 23, 28, 38},
    [SIDE_OFF]    = {50, 50, 50, 50, 50},
};

const uint8_t side_light_table[7] = {
    0,
    14,
    22,
    34,
    55,
    79,
    106,
};

const uint8_t side_led_index_tab[SIDE_LINE][2] = {
    {5, 6},
    {4, 7},
    {3, 8},
    {2, 9},
    {1, 10},
    {0, 11},
};

void     rgb_matrix_update_pwm_buffers(void);

void set_sys_light(void) {
    sys_light = user_config.ee_side_light > 5 ? 1 : (3 - user_config.ee_side_light / 2);
    current_rgb.r = current_rgb.r / sys_light;
    current_rgb.g = current_rgb.g / sys_light;
    current_rgb.b = current_rgb.b / sys_light;
}

void clear_rgb(void) {
    rgb_matrix_set_color_all(RGB_OFF);
    rgb_matrix_update_pwm_buffers();
}


/**
 * @brief  refresh side leds.
 */
void side_ws2812_refresh(void) {
    if (!is_side_ws2812_off() || (user_config.ee_side_light > 0 && user_config.ee_side_mode != SIDE_OFF)) {
        pwr_side_led_on(); // power on side LED before refresh
    }
    side_ws2812_flush();
}

void signal_sleep(void) {
    uint8_t r, g, b;
    r = 0x00; g = 0x00; b = 0x80;
    if (dev_info.link_mode == LINK_RF_24) {
        g = 0x80;
        b = 0x00;
    }
    pwr_side_led_on();
    wait_ms(10);
    side_ws2812_set_color_strip(LEFT_SIDE + RIGHT_SIDE, r, g, b);
    side_ws2812_refresh();
    wait_ms(140);
}


/**
 * @brief  Adjusting the brightness of side lights.
 * @param  bright: 0 - decrease, 1 - increase.
 * @note  save to eeprom.
 */
void side_light_control(uint8_t bright) {
    if (bright) {
        if (user_config.ee_side_light < SIDE_BRIGHT_MAX) { user_config.ee_side_light++; }
    } else {
        if (user_config.ee_side_light > 0) { user_config.ee_side_light--; }
    }
#ifndef NO_DEBUG
    dprintf("side matrix light_control [NOEEPROM]: %d\n", user_config.ee_side_light);
#endif
}

/**
 * @brief  Adjusting the speed of side lights.
 * @param  fast: 0 - decrease, 1 - increase.
 * @note  save to eeprom.
 */
void side_speed_control(uint8_t fast) {
    if (fast) {
        if (user_config.ee_side_speed > 0) { user_config.ee_side_speed--; }
    } else {
        if (user_config.ee_side_speed < SIDE_SPEED_MAX) { user_config.ee_side_speed++; }
    }
#ifndef NO_DEBUG
    dprintf("side matrix speed_control [NOEEPROM]: %d\n", user_config.ee_side_speed);
#endif
}

/**
 * @brief  Switch to the next color of side lights.
 * @param  color: 0 - prev, 1 - next.
 * @note  save to eeprom.
 */
void side_colour_control(uint8_t color) {
    if (user_config.ee_side_mode != SIDE_WAVE) {
        if (user_config.ee_side_rgb) {
            user_config.ee_side_rgb    = 0;
            user_config.ee_side_colour = game_mode_enable;
        }
    }

    if (user_config.ee_side_rgb) {
        user_config.ee_side_rgb    = 0;
        user_config.ee_side_colour = color ? game_mode_enable : SIDE_COLOUR_MAX - 1;
    } else {
        if (color) { user_config.ee_side_colour++; }
        else { user_config.ee_side_colour--; }

        if (user_config.ee_side_colour >= SIDE_COLOUR_MAX) {
            user_config.ee_side_rgb    = 1;
            user_config.ee_side_colour = game_mode_enable;
        }
    }
#ifndef NO_DEBUG
    dprintf("side matrix colour_control [NOEEPROM]: %d rgb: %d\n", user_config.ee_side_colour, user_config.ee_side_rgb);
#endif
}

/**
 * @brief  Change the color mode of side lights.
 * @param  dir: 0 - prev, 1 - next.
 * @note  save to eeprom.
 */
void side_mode_control(uint8_t dir) {
    if (dir) {
        user_config.ee_side_mode = (user_config.ee_side_mode + 1) % (SIDE_OFF + 1);
    } else {
        if (user_config.ee_side_mode > 0) { user_config.ee_side_mode--; }
        else { user_config.ee_side_mode = SIDE_OFF; }
    }
    side_play_point  = 0;
#ifndef NO_DEBUG
    dprintf("side matrix mode_control [NOEEPROM]: %d\n", user_config.ee_side_mode);
#endif
}

/**
 * @brief  sys sw led show.
 */
void sys_sw_led_show(void) {
    if (sys_show_timer == 0) { return; }
    if (dev_info.sys_sw_state == SYS_SW_MAC) {
        current_rgb.r = 0x80;
        current_rgb.g = 0x80;
        current_rgb.b = 0x80;
    } else {
        current_rgb.r = 0x00;
        current_rgb.g = 0x00;
        current_rgb.b = 0x80;
    }

    if (timer_elapsed32(sys_show_timer) >= 2900) {
        sys_show_timer = 0;
    }

    if ((timer_elapsed32(sys_show_timer) / 500) % 2 == 0) {
        set_sys_light();
        side_ws2812_set_color_strip(RIGHT_SIDE, current_rgb.r, current_rgb.g, current_rgb.b);
    } else {
        side_ws2812_set_color_strip(RIGHT_SIDE, RGB_OFF);
    }
}

/**
 * @brief  sleep_sw_led_show.
 */
void sleep_sw_led_show(void) {
    if (sleep_show_timer == 0) { return; }
    current_rgb.r = 0x00, current_rgb.g = 0x00, current_rgb.b = 0x00;
    switch (user_config.sleep_mode) {
        case 0:
            current_rgb.r = 0x80;
            break;
        case 1:
            current_rgb.g = 0x80;
            break;
        case 2:
            current_rgb.r = 0x80;
            current_rgb.g = 0x80;
            break;
    }

    if (timer_elapsed32(sleep_show_timer) >= 2900) {
        sleep_show_timer = 0;
    }

    if ((timer_elapsed32(sleep_show_timer) / 500) % 2 == 0) {
        set_sys_light();
        side_ws2812_set_color_strip(RIGHT_SIDE, current_rgb.r, current_rgb.g, current_rgb.b);
    } else {
        side_ws2812_set_color_strip(RIGHT_SIDE, RGB_OFF);
    }
}

/**
 * @brief  sys_led_show.
 */
void sys_led_show(void) {
    current_rgb.g = 0x80, current_rgb.b = 0x80, current_rgb.r = 0x00;
    uint8_t led_side = LEFT_SIDE;
    if (host_keyboard_led_state().caps_lock) {
        led_side = RIGHT_SIDE;
        set_sys_light();
        side_ws2812_set_color_strip(LEFT_SIDE, current_rgb.r, current_rgb.g, current_rgb.b);
    }

    if (user_config.numlock_state != 1 || !host_keyboard_led_state().num_lock) { return; }
    
    current_rgb.r = 0x80, current_rgb.g = 0x80, current_rgb.b = 0x80;
    set_sys_light();
    side_ws2812_set_color_strip(led_side, current_rgb.r, current_rgb.g, current_rgb.b);
        
}

static uint8_t breathe_data(uint8_t step) {
    uint8_t value     = 0;
    uint8_t adjust    = 8;
    uint8_t max_value = BREATHE_TAB_LEN - 1;
    if (step < max_value / 2) {
        value = ((step * adjust / 21) * step) / adjust + step;
    } else if (step  < BREATHE_TAB_LEN / 2  + 1) {
        value = 255;
    } else {
        value = ((((max_value - step) * adjust) / 21) * (max_value - step)) / adjust + (max_value - step);
    }
    return value;
}

/**
 * @brief  light_point_playing.
 * @param trend:
 * @param step:
 * @param len:
 * @param point:
 */
static void light_point_playing(uint8_t trend, uint8_t step, uint8_t len, uint8_t *point) {
    if (trend) {
        *point += step;
        if (*point > 254 && len == 0) { *point = 0; }
        else if (*point >= len) { *point -= len; }
    } else {
        *point -= step;
        if (*point < 1 && len == 0) { *point = 255; }
        else if (*point >= len) { *point = len - (255 - *point) - 1; }
    }
}


/**
 * @brief  count_rgb_light.
 * @param light_temp:
 */
static void count_rgb_light(uint8_t light_temp) {
    uint16_t temp;

    temp   = (light_temp)*current_rgb.r + current_rgb.r;
    current_rgb.r = temp >> 8;

    temp   = (light_temp)*current_rgb.g + current_rgb.g;
    current_rgb.g = temp >> 8;

    temp   = (light_temp)*current_rgb.b + current_rgb.b;
    current_rgb.b = temp >> 8;

}

/**
 * @brief  side_wave_mode_show.
 */
static void side_wave_mode_show(void) {
    uint8_t play_index;

    if (user_config.ee_side_rgb) {
        light_point_playing(0, 6, 0, &side_play_point);
    } else {
        light_point_playing(0, 2, BREATHE_TAB_LEN, &side_play_point);
    }

    play_index = side_play_point;
    for (uint8_t i = 0; i < SIDE_LINE; i++) {
        if (user_config.ee_side_rgb) {
            hsv.h = play_index;
            current_rgb = hsv_to_rgb_nocie(hsv);
            light_point_playing(1, 32, 0, &play_index);
        } else {
            current_rgb.r = colour_lib[user_config.ee_side_colour][0];
            current_rgb.g = colour_lib[user_config.ee_side_colour][1];
            current_rgb.b = colour_lib[user_config.ee_side_colour][2];

            light_point_playing(1, 12, BREATHE_TAB_LEN, &play_index);
            count_rgb_light(breathe_data(play_index));
        }

        count_rgb_light(side_light_table[user_config.ee_side_light]);

        for (int8_t j = 0; j < 2; j++) {
            side_ws2812_set_color(side_led_index_tab[i][j], current_rgb.r, current_rgb.g, current_rgb.b);
        }
    }
}

/**
 * @brief  side_spectrum_mode_show.
 */
static void side_spectrum_mode_show(void) {
    light_point_playing(0, 2, 0, &side_play_point);
    hsv.h = side_play_point;
    current_rgb   = hsv_to_rgb_nocie(hsv);

    count_rgb_light(side_light_table[user_config.ee_side_light]);
    side_ws2812_set_color_strip(LEFT_SIDE + RIGHT_SIDE, current_rgb.r, current_rgb.g, current_rgb.b);
}

/**
 * @brief  side_breathe_mode_show.
 */
static void side_breathe_mode_show(void) {
    uint8_t mod_side_light = user_config.ee_side_light == 1 ? 2 : user_config.ee_side_light;

    light_point_playing(1, 1, BREATHE_TAB_LEN, &side_play_point);

    current_rgb.r = colour_lib[user_config.ee_side_colour][0];
    current_rgb.g = colour_lib[user_config.ee_side_colour][1];
    current_rgb.b = colour_lib[user_config.ee_side_colour][2];

    count_rgb_light(breathe_data(side_play_point));
    count_rgb_light(side_light_table[mod_side_light]);

    side_ws2812_set_color_strip(LEFT_SIDE + RIGHT_SIDE, current_rgb.r, current_rgb.g, current_rgb.b);
}

/**
 * @brief  side_static_mode_show.
 */
static void side_static_mode_show(void) {
    current_rgb.r = colour_lib[user_config.ee_side_colour][0];
    current_rgb.g = colour_lib[user_config.ee_side_colour][1];
    current_rgb.b = colour_lib[user_config.ee_side_colour][2];
 
    count_rgb_light(side_light_table[user_config.ee_side_light]);
    side_ws2812_set_color_strip(LEFT_SIDE + RIGHT_SIDE, current_rgb.r, current_rgb.g, current_rgb.b);
}

/**
 * @brief  side_off_mode_show.
 */
static void side_off_mode_show(void) {
    side_ws2812_set_color_strip(LEFT_SIDE + RIGHT_SIDE, RGB_OFF);
}

/**
 * @brief  side_one_control
 */
void side_one_control(void)
{

    user_config.ee_side_one = (user_config.ee_side_one + 1) % 4;
    side_one_timer = 0;
#ifndef NO_DEBUG
    dprintf("side matrix side_control [NOEEPROM]: %d\n", user_config.ee_side_one);
#endif
}

/**
 * @brief  side_one_show
 */
static void side_one_show(void)
{
    if (game_mode_enable || user_config.ee_side_one == 0 || user_config.ee_side_light == 0) { return; }
    static uint8_t my_side = RIGHT_SIDE;

    if (user_config.ee_side_one != LEFT_SIDE + RIGHT_SIDE) {
        side_ws2812_set_color_strip(user_config.ee_side_one, RGB_OFF);
    } else {
       if (side_one_timer < 50 ) {
            side_ws2812_set_color_strip(LEFT_SIDE + RIGHT_SIDE, RGB_OFF);
            return;
        } else if ( side_one_timer > 6000) {
            side_one_timer = 50;
            my_side = my_side == RIGHT_SIDE ? LEFT_SIDE : RIGHT_SIDE;
        }

        side_ws2812_set_color_strip(my_side, RGB_OFF);
    }
}

/**
 * @brief  rf_led_show.
 */
void rf_led_show(void) {
    static uint32_t rf_blink_timer = 0;
    uint16_t        rf_blink_period = 0;

    if (rf_blink_cnt || (rf_link_show_time < RF_LINK_SHOW_TIME)) {
        current_rgb.r = dev_info.link_mode == LINK_USB ? 0x80 : 0x00;     // LINK_USB
        current_rgb.g = dev_info.link_mode % LINK_USB == 0 ? 0x80 : 0x00; // LINK_USB || LINK_RF_24
        current_rgb.b = dev_info.link_mode % LINK_USB != 0 ? 0x80 : 0x00; // LINK_BT
    } else {
        return;
    }

    if (rf_blink_cnt) {
        if (dev_info.rf_state == RF_PAIRING) {
            rf_blink_period = RF_LED_PAIR_PERIOD;
        } else {
            rf_blink_period = RF_LED_LINK_PERIOD;
        }

        if (timer_elapsed32(rf_blink_timer) > (rf_blink_period >> 1)) {
            current_rgb.r = 0x00, current_rgb.g = 0x00, current_rgb.b = 0x00;
        }

        if (timer_elapsed32(rf_blink_timer) >= rf_blink_period) {
            rf_blink_cnt--;
            rf_blink_timer = timer_read32();
        }
    }

    set_sys_light();
    side_ws2812_set_color_strip(LEFT_SIDE, current_rgb.r, current_rgb.g, current_rgb.b);
    // light up corresponding BT/RF key
    if (dev_info.link_mode <= LINK_BT_3) {
        uint8_t my_pos = dev_info.link_mode == LINK_RF_24 ? 4 : dev_info.link_mode;
        if (rf_link_show_time > RF_LINK_SHOW_TIME - 10) {
            rgb_matrix_set_color(led_idx.KC_GRV - my_pos, RGB_OFF);
        } else {
            uint8_t my_pos = dev_info.link_mode == LINK_RF_24 ? 4 : dev_info.link_mode;
            rgb_required = 1;
            rgb_matrix_set_color(led_idx.KC_GRV - my_pos, current_rgb.r, current_rgb.g, current_rgb.b);
        }
    }
}

/**
 * @brief  bat_num_led.
 */
void bat_num_led(void)
{
    if (!f_bat_num_show) { return; }
    uint8_t bat_percent = dev_info.rf_battery;
    uint8_t bat_pct = bat_percent / 10;
    uint8_t r, g, b;

    rgb_required = 1;
    // set color
    
    if (bat_percent < low_bat_level) {
        r = 0xff; g = 0x00; b = 0x00;
    }
    else if (bat_percent <= 50) {
        r = 0xff; g = 0x40; b = 0x00;
    }
    else if (bat_percent <= 80) {
        r = 0xff; g = 0xff; b = 0x00;
    }
    else {
        r = 0x00; g = 0xff; b = 0x00;
    }

    if (bat_percent % 10 == 0) { bat_pct--; }

    for(uint8_t i=0; i < bat_pct; i++) {
        rgb_matrix_set_color(29 - i, r, g, b);
    }


    // set percent
    if (bat_percent % 10 == 0) {
        r = 0x00; g = 0xff; b = 0x00;
    } else if (bat_percent % 10 <= 3) {
        r = 0xff; g = 0x00; b = 0x00;
    } else if (bat_percent % 10 <= 6) {
        r = 0xff; g = 0x40; b = 0x00;
    } else {
        r = 0x00; g = 0xff; b = 0x00;
    }

    rgb_matrix_set_color(29 - bat_pct, r, g, b);
}

/**
 * @brief  bat_percent_led.
 */
void bat_percent_led(void) {
    uint8_t bat_percent = dev_info.rf_battery;
    uint8_t bat_end_led = 5;
    current_rgb.r = 0x80, current_rgb.g = 0x40, current_rgb.b = 0x00;

    if (bat_percent <= 30) {
        bat_end_led = 1;
        current_rgb.r = 0x80, current_rgb.g = 0x00, current_rgb.b = 0x00;
    } else if (bat_percent <= 40) {
        bat_end_led = 1;
    } else if (bat_percent <= 60) {
        bat_end_led = 2;
    } else if (bat_percent <= 70) {
        bat_end_led = 3;
    } else if (bat_percent <= 80) {
        bat_end_led = 4;
    } else {
        current_rgb.r = 0x00, current_rgb.g = 0x80, current_rgb.b = 0x00;
    }

    set_sys_light();
    side_ws2812_set_color_strip(RIGHT_SIDE, current_rgb.r, current_rgb.g, current_rgb.b);

    for (uint8_t i = bat_end_led + 1; i < 6; i++) {
        side_ws2812_set_color(11 - i, RGB_OFF);
    }
}


/**
 * @brief  bat_led_show.
 */

void bat_led_show(void) {
    static bool bat_show_flag   = 1;
    static bool bat_show_breath = 0;
    static bool f_init          = 1;

    static uint8_t  bat_play_point = 0;
    static uint32_t bat_play_timer = 0;
    static uint32_t bat_show_time  = 0;

    static uint32_t bat_sts_debounce = 0;
    static uint8_t  charge_state     = 0;

    if (f_init) {
        f_init        = 0;
        bat_show_time = timer_read32();
        charge_state  = dev_info.rf_charge;
    }

    if (game_mode_enable) {
        if(dev_info.rf_battery < low_bat_level) {
            side_ws2812_set_color_strip(RIGHT_SIDE, 0x40, 0x00, 0x00);
        }
        return;
    }

    if (charge_state != dev_info.rf_charge) {
        if (timer_elapsed32(bat_sts_debounce) > 1000) {
            if ((charge_state & 0x01) == 0 && (dev_info.rf_charge & 0x01) != 0) {
                bat_show_flag   = true;
                bat_show_breath = true;
                bat_show_time   = timer_read32();
            }
            charge_state = dev_info.rf_charge;
        }
    } else {
        bat_sts_debounce = timer_read32();
        if (timer_elapsed32(bat_show_time) > 5000) {
            bat_show_flag   = false;
            bat_show_breath = false;
        }
        if (charge_state == 0x03) {
            bat_show_breath = true;
        } else if (charge_state & 0x01) {
            dev_info.rf_battery = 100;
        }
    }

    if (dev_info.rf_battery < low_bat_level) {
        bat_show_flag   = true;
        bat_show_breath = true;
        bat_show_time = timer_read32();
    }


    if (f_bat_hold || bat_show_flag) {
        if (bat_show_breath) {
            if (timer_elapsed32(bat_play_timer) > 10) {
                bat_play_timer = timer_read32();
                light_point_playing(0, 1, BREATHE_TAB_LEN, &bat_play_point);
            }
            current_rgb.r = 0x80, current_rgb.g = dev_info.rf_battery < low_bat_level ? 0x00 : 0x40, current_rgb.b = 0x00;
            count_rgb_light(breathe_data(bat_play_point));
            set_sys_light();
            side_ws2812_set_color_strip(RIGHT_SIDE, current_rgb.r, current_rgb.g, current_rgb.b);
        } else {
            bat_percent_led();
        }
    }
}


/**
 * @brief  device_reset_show.
 */
void device_reset_show(void) {

    pwr_rgb_led_on();
    pwr_side_led_on();

    for (uint8_t blink_cnt = 0; blink_cnt < 3; blink_cnt++) {
        rgb_matrix_set_color_all(0x40, 0x40, 0x40);
        side_ws2812_set_color_strip(LEFT_SIDE + RIGHT_SIDE, 0x40, 0x40, 0x40);
        rgb_matrix_update_pwm_buffers();
        side_ws2812_refresh();
        wait_ms(200);

        side_ws2812_set_color_strip(LEFT_SIDE + RIGHT_SIDE, RGB_OFF);
        clear_rgb();
        side_ws2812_refresh();
        wait_ms(200);
    }
}


/**
 * @brief  device_reset_init.
 */
void device_reset_init(void) {
    side_play_point  = 0;
    game_mode_enable = 0;
    f_bat_hold       = false;

    rgb_matrix_enable_noeeprom();
    user_config_reset();
}


/**
 *       RGB test
*/
void rgb_test_show(void) {
    // open power control
    pwr_rgb_led_on();
    pwr_side_led_on();

    for (uint8_t i = 0; i < SIDE_COLOUR_MAX; i++) {
        rgb_matrix_set_color_all(colour_lib[i][0], colour_lib[i][1], colour_lib[i][2]);
        rgb_matrix_update_pwm_buffers();
        side_ws2812_set_color_strip(LEFT_SIDE + RIGHT_SIDE, colour_lib[i][0], colour_lib[i][1], colour_lib[i][2]);
        side_ws2812_refresh();
        wait_ms(500);
    }
    clear_rgb();
}

void signal_rgb_led(uint8_t selected_color, uint8_t start_led, uint8_t end_led, uint16_t show_time) {
    rgb_color           = selected_color;
    rgb_start_led       = start_led;
    rgb_end_led         = end_led > RGB_MATRIX_LED_COUNT ? start_led : end_led;
    rgb_show_time       = show_time;
    rgb_indicator_timer = timer_read32();
}

void rgb_led_indicator(void) {
    if (rgb_show_time == 0) { return; }
    if (timer_elapsed32(rgb_indicator_timer) < rgb_show_time || rgb_show_time == UINT16_MAX) {
        current_rgb.r = colour_lib[rgb_color][0];
        current_rgb.g = colour_lib[rgb_color][1];
        current_rgb.b = colour_lib[rgb_color][2];

        rgb_required  = 2;
        for (uint8_t i = rgb_start_led; i <= rgb_end_led; i++) {
            rgb_matrix_set_color(i, current_rgb.r, current_rgb.g, current_rgb.b);
        }
    } else {
        for (uint8_t i = rgb_start_led; i <= rgb_end_led; i++) {
            rgb_matrix_set_color(i, RGB_OFF);
        }
        rgb_show_time       = 0;
        rgb_indicator_timer = 0;
    }
}


void caps_word_show(void) {
    static bool caps_word_rgb_on = 0;
    if (!is_caps_word_on() || game_mode_enable || !user_config.caps_word_enable) {
        if (caps_word_rgb_on) {
            caps_word_rgb_on = 0;
            rgb_matrix_set_color(led_idx.KC_CAPS, RGB_OFF);
        }
        return;
    } else {
        rgb_required     = 2;
        caps_word_rgb_on = 1;
        rgb_matrix_set_color(led_idx.KC_CAPS, RGB_CYAN);
    }
}

void numlock_rgb_show(void) {
    static bool num_lock_rgb_on = 0;
    if (!host_keyboard_led_state().num_lock || user_config.numlock_state != 2) {
        if (num_lock_rgb_on) {
            num_lock_rgb_on = 0;
            rgb_matrix_set_color(led_idx.KC_NUM, RGB_OFF);
        }
        return;
    } else {
        rgb_required    = 2;
        num_lock_rgb_on = 1;
        rgb_matrix_set_color(led_idx.KC_NUM, RGB_WHITE);
    }
}

void rgb_matrix_step_game_mode(uint8_t step) {
    if (step) {
        user_config.game_rgb_mod++;
        if (user_config.game_rgb_mod > RGB_MATRIX_CUSTOM_GAME_KEYS) { user_config.game_rgb_mod = 1; }
        else if (user_config.game_rgb_mod > 3) { user_config.game_rgb_mod = RGB_MATRIX_CUSTOM_GAME_KEYS; }
    } else {
        user_config.game_rgb_mod--;
        if (user_config.game_rgb_mod > 3) { user_config.game_rgb_mod = 3; }
        else if (user_config.game_rgb_mod < 1) { user_config.game_rgb_mod = RGB_MATRIX_CUSTOM_GAME_KEYS; }
    }

    rgb_matrix_mode_noeeprom(user_config.game_rgb_mod);
}

/**
 * @brief  side_led_show.
 */
void normal_led_process(void) {
    static uint32_t side_update_time  = 0;

    // side_mode & side_speed should always be valid...
    // refresh side LED animation based on speed.

    uint32_t update_interval = game_mode_enable ? 500 : side_speed_table[user_config.ee_side_mode][user_config.ee_side_speed];

    if (timer_elapsed32(side_update_time) < update_interval) { return; }
    side_update_time = timer_read32();


    if (user_config.ee_side_light == 0) {
        side_off_mode_show();
    } else {

        switch (user_config.ee_side_mode) {
            case SIDE_WAVE:
                side_wave_mode_show();
                break;
            case SIDE_MIX:
                side_spectrum_mode_show();
                break;
            case SIDE_BREATH:
                side_breathe_mode_show();
                break;
            case SIDE_STATIC:
                side_static_mode_show();
                break;
            case SIDE_OFF:
                side_off_mode_show();
                break;
        }
    }
    
    side_one_show();
    bat_led_show();
    sleep_sw_led_show();

    sys_sw_led_show();
    sys_led_show();
}

void realtime_led_process(void) {
    rf_led_show();
    rgb_led_indicator();
    caps_word_show();
    numlock_rgb_show();
    bat_num_led();
}

void led_show(void) {
    if (f_wakeup_prepare) { return; }
    side_ws2812_refresh();
    normal_led_process();
    realtime_led_process();
}

