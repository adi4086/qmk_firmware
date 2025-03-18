RGB_MATRIX_EFFECT(GAME_KEYS)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static bool GAME_KEYS(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    RGB rgb = rgb_matrix_hsv_to_rgb(rgb_matrix_config.hsv);

    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        switch (i) {
            case 0:    // ESC
            case 39:   // W
            case 56:   // A
            case 57:   // S
            case 58:   // D
            case 83:   // Up
            case 95:   // Left
            case 96:   // Down
            case 97:   // Right
                rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
                break;

            default:
                rgb_matrix_set_color(i, 0x00, 0x00, 0x00);
                break;
        }
    }

    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
