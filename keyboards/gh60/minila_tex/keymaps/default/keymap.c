#include "minila_tex.h"
#include QMK_KEYBOARD_H
#include "keymap_spanish.h"
#ifndef RGBLIGHT_ENABLE
#define RGBLIGHT_ENABLE
#endif
#ifdef RGBLIGHT_ENABLE
#    include "rgblight.h"
#endif

// Layers definitions
#define _BL 0
#define _FL 1
#define _ML 2
#define _QL 3

static uint8_t old_layer = 255;

// RGB sleep when not in use variables
long start = 0;
bool timer_led = true;

typedef struct {
    bool is_press_action;
    int  state;
} tap;

enum {
    SINGLE_TAP        = 1,
    SINGLE_HOLD       = 2,
    DOUBLE_TAP        = 3,
    DOUBLE_HOLD       = 4,
    DOUBLE_SINGLE_TAP = 5,  // send two single taps
    TRIPLE_TAP        = 6,
    TRIPLE_HOLD       = 7
};

// Tap dance enums
enum {
    X_LSFT = 0,
    X_RSFT = 1
};

// Layers control
bool layer_fixed = false;
uint8_t fixed_layer = 0;
enum custom_keycodes {
    LAYER_FIX = SAFE_RANGE,
    M_LAYER_1,
    M_LAYER_2,
    M_LAYER_3
};

int cur_dance(qk_tap_dance_state_t *state);

// for the left shift tap dance. Put it here so it can be used in any keymap
void lsft_finished(qk_tap_dance_state_t *state, void *user_data);
void lsft_reset(qk_tap_dance_state_t *state, void *user_data);

// for the right shift tap dance. Put it here so it can be used in any keymap
void rsft_finished(qk_tap_dance_state_t *state, void *user_data);
void rsft_reset(qk_tap_dance_state_t *state, void *user_data);

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	//Keymap _BL: Base Layer (Default Layer)
    [_BL] = KEYMAP(
        KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, ES_APOS, ES_IEXL, KC_DEL, KC_BSPC,
        KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, ES_GRV, ES_PLUS, ES_PLUS,
        KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, ES_NTIL, ES_ACUT, ES_CCED, KC_ENT,
        TD(X_LSFT), KC_Z, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, ES_MINS, ES_LESS, KC_UP, TD(X_RSFT),
        M_LAYER_1, KC_LCTL, KC_LALT, KC_LGUI, KC_SPC, KC_RGUI, KC_RALT, KC_APP, KC_LEFT, KC_DOWN, KC_RGHT),

    // Keymap _FL: Function Layer
    [_FL] = KEYMAP(
        KC_GRAVE, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_BSPC, KC_DEL,
        LAYER_FIX, _______, KC_MS_UP, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, KC_MS_LEFT, KC_MS_DOWN, KC_MS_RIGHT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_PGUP, _______,
        _______, M_LAYER_2, _______, _______, _______, KC_MS_BTN1, KC_MS_BTN2, _______, KC_HOME, KC_PGDOWN, KC_END),

    // Keymap _ML: Media Layer
    [_ML] = KEYMAP(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_BRIU, KC_BRIU, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, KC_MUTE, _______, _______, KC_BRMD, _______, KC_VOLU, _______,
        _______, _______, M_LAYER_3, _______, KC_MPLY, _______, _______, _______, KC_MPRV, KC_VOLD, KC_MNXT),

    // Keymap _QL: QMK Layer
    [_QL] = KEYMAP(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, EEPROM_RESET, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, RESET, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, RGB_VAI, _______,
        _______, _______, _______, _______, RGB_M_P, _______, _______, _______, RGB_MODE_REVERSE, RGB_VAD, RGB_MODE_FORWARD),
};

/*void suspend_power_down_user(void) {
    rgb_matrix_set_suspend_state(true);
}

void suspend_wakeup_init_user(void) {
    rgb_matrix_set_suspend_state(false);
}*/

void persistent_default_layer_set(uint16_t default_layer) {
    eeconfig_update_default_layer(default_layer);
    default_layer_set(default_layer);
}

void keyboard_pre_init_user(void) {
    // Happens before most anything is started. Good for hardware setup that you want running very early.
    setPinOutput(CAPSLOCK_PIN);
}

/*void matrix_init_user(void) {
    // Happens midway through the firmware's startup process. Hardware is initialized, but features may not be yet.
    // rgblight_enable();
}*/

void matrix_scan_user(void) {
    // Whenever possible you should customize your keyboard by using process_record_*()
    // and hooking into events that way, to ensure that your code does not have a negative performance impact on your keyboard.
    // However, in rare cases it is necessary to hook into the matrix scanning
#ifdef RGBLIGHT_ENABLE

    // rgblight_mode(RGBLIGHT_MODE_ALTERNATING);

    // RGB sleep when not in use START
    if (timer_elapsed(start) > 60000) {
        start = 0;
        timer_clear();
        timer_led = false;
        rgblight_disable_noeeprom();
    }
    // RGB sleep when not in use END

    uint8_t new_layer = biton32(layer_state);

    if (old_layer != new_layer) {
        switch (new_layer) {
            case _BL:
                rgblight_sethsv_noeeprom(28,255,rgblight_get_val());
                break;
            case _FL:
                rgblight_sethsv_noeeprom(170,255,rgblight_get_val());
                break;
            case _ML:
                rgblight_sethsv_noeeprom(123,255,rgblight_get_val());
                break;
            case _QL:
                rgblight_sethsv_noeeprom(191,255,rgblight_get_val());
                break;
            default:
                rgblight_sethsv_noeeprom(0,255,rgblight_get_val());
                break;
        }

        old_layer = new_layer;
    }

#endif  // RGBLIGHT_ENABLE
}

void led_set_user(uint8_t usb_led) {
    // This function will be called when the state of one of the host LED state changes
    if (IS_LED_ON(usb_led, USB_LED_CAPS_LOCK)) {
        writePinLow(CAPSLOCK_PIN);
    } else {
        writePinHigh(CAPSLOCK_PIN);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // This is called by QMK during key processing before the actual key event is handled.

    // RGB sleep when not in use START
    start = timer_read();
    timer_led = true;
    rgblight_enable_noeeprom();
    // RGB sleep when not in use END

    if (layer_fixed && (keycode == LAYER_FIX)) {
        if (record->event.pressed) {
        // when keycode is pressed
            layer_clear();
            layer_fixed = false;
            fixed_layer = 0;
        } else {
        // when keycode is released
        }
        return false;
    }
    else if (!layer_fixed) {
        switch (keycode) {
            case LAYER_FIX:
                if (record->event.pressed) {
                    if (!layer_fixed) {
                        layer_fixed = true;
                        fixed_layer = biton32(layer_state);
                    }
                }
                return false;
                break;
            case M_LAYER_1:
                if (record->event.pressed) {
                    layer_on(1);
                } else {
                    layer_off(1);
                }
                return false;
                break;
            case M_LAYER_2:
                if (record->event.pressed) {
                    layer_on(2);
                } else {
                    layer_off(2);
                }
                return false;
                break;
            case M_LAYER_3:
                if (record->event.pressed) {
                    layer_on(3);
                } else {
                    layer_off(3);
                }
                return false;
                break;
        }
    }
    return true;
}

int cur_dance(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        if (!state->pressed) return SINGLE_TAP;
        // key has not been interrupted, but they key is still held. Means you want to send a 'HOLD'.
        else
            return SINGLE_HOLD;
    } else if (state->count == 2) {
        /*
         * DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
         * action when hitting 'pp'. Suggested use case for this return value is when you want to send two
         * keystrokes of the key, and not the 'double tap' action/macro.
         */
        if (state->interrupted)
            return DOUBLE_SINGLE_TAP;
        else if (state->pressed)
            return DOUBLE_HOLD;
        else
            return DOUBLE_TAP;
    }
    // Assumes no one is trying to type the same letter three times (at least not quickly).
    // If your tap dance key is 'KC_W', and you want to type "www." quickly - then you will need to add
    // an exception here to return a 'TRIPLE_SINGLE_TAP', and define that enum just like 'DOUBLE_SINGLE_TAP'
    if (state->count == 3) {
        if (state->interrupted || !state->pressed)
            return TRIPLE_TAP;
        else
            return TRIPLE_HOLD;
    } else
        return 8;  // magic number. At some point this method will expand to work for more presses
}

// instanalize an instance of 'tap' for the 'x' tap dance.
static tap xtap_state = {.is_press_action = true, .state = 0};

void lsft_finished(qk_tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case SINGLE_HOLD:
            register_code(KC_LSFT);
            break;
        case SINGLE_TAP:
            register_code(KC_LSFT);
            register_code(KC_8);
            break;
        case DOUBLE_TAP:
            register_code(KC_LALT);
            register_code(ES_ACUT);
            break;
        case TRIPLE_TAP:
            register_code(KC_LALT);
            register_code(ES_GRV);
    }
}

void lsft_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (xtap_state.state) {
        case SINGLE_HOLD:
            unregister_code(KC_LSFT);
            break;
        case SINGLE_TAP:
            unregister_code(KC_LSFT);
            unregister_code(KC_8);
            break;
        case DOUBLE_TAP:
            unregister_code(KC_LALT);
            unregister_code(ES_ACUT);
            break;
        case TRIPLE_TAP:
            unregister_code(KC_LALT);
            unregister_code(ES_GRV);
    }
    xtap_state.state = 0;
}

void rsft_finished(qk_tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case SINGLE_HOLD:
            register_code(KC_LSFT);
            break;
        case SINGLE_TAP:
            register_code(KC_LSFT);
            register_code(KC_9);
            break;
        case DOUBLE_TAP:
            register_code(KC_LALT);
            register_code(ES_CCED);
            break;
        case TRIPLE_TAP:
            register_code(KC_LALT);
            register_code(ES_PLUS);
    }
}

void rsft_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (xtap_state.state) {
        case SINGLE_HOLD:
            unregister_code(KC_LSFT);
            break;
        case SINGLE_TAP:
            unregister_code(KC_LSFT);
            unregister_code(KC_9);
            break;
        case DOUBLE_TAP:
            unregister_code(KC_LALT);
            unregister_code(ES_CCED);
            break;
        case TRIPLE_TAP:
            unregister_code(KC_LALT);
            unregister_code(ES_PLUS);
    }
    xtap_state.state = 0;
}

qk_tap_dance_action_t tap_dance_actions[] = {
    [X_LSFT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, lsft_finished, lsft_reset),
    [X_RSFT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, rsft_finished, rsft_reset)
};
