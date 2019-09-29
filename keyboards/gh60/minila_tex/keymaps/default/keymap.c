#include "minila_tex.h"
#include QMK_KEYBOARD_H
#include "keymap_spanish.h"
#ifdef RGBLIGHT_ENABLE
#include "rgblight.h"
#endif

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
#define _BL 0
#define _FL 1

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	//Keymap _BL: Base Layer (Default Layer)
	[_BL] = KEYMAP(
		KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, ES_APOS, ES_IEXL, KC_DEL, KC_BSPC, 
		KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, ES_GRV, ES_PLUS, ES_PLUS, 
		KC_LCAP, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, ES_NTIL, ES_ACUT, ES_CCED, KC_ENT, 
		KC_LSFT, KC_Z, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, ES_MINS, ES_LESS, KC_UP, KC_RSFT, 
		MO(1), KC_LCTL, KC_LALT, KC_LGUI, KC_SPC, MO(1), KC_RGUI, KC_RALT, KC_LEFT, KC_DOWN, KC_RGHT),

	//Keymap _FL: Function Layer
	[_FL] = KEYMAP(
		KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, ES_APOS, ES_IEXL, KC_DEL, KC_BSPC, 
		KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, ES_GRV, ES_PLUS, ES_PLUS, 
		KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, ES_NTIL, ES_ACUT, ES_CCED, KC_ENT, 
		KC_LSFT, KC_Z, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, ES_MINS, ES_LESS, KC_UP, KC_RSFT, 
		MO(1), KC_LCTL, KC_LALT, KC_LGUI, KC_SPC, MO(1), KC_RGUI, KC_RALT, KC_LEFT, KC_DOWN, KC_RGHT),

};

void persistent_default_layer_set(uint16_t default_layer) {
  eeconfig_update_default_layer(default_layer);
  default_layer_set(default_layer);
}

void matrix_init_user(void) {
  rgblight_enable();
}

void keyboard_pre_init_user(void) {
  // Call the keyboard pre init code.

  // Set our LED pins as output
  setPinOutput(CAPSLOCK_PIN);
}

void matrix_scan_user(void) {
  #ifdef RGBLIGHT_ENABLE

  static uint8_t old_layer = 255;
  uint8_t new_layer = biton32(layer_state);

  if (old_layer != new_layer) {
    switch (new_layer) {
      case _BL:
        rgblight_setrgb(0x00, 0x00, 0xFF);
        break;
      case _FL:
        rgblight_setrgb(0x00, 0xA0, 0xFF);
        break;
	  default:
	  	rgblight_setrgb(0xFF, 0x00, 0x00);
        break;
    }

    old_layer = new_layer;
  }

  #endif //RGBLIGHT_ENABLE
}

void led_set_user(uint8_t usb_led) {
    /*if (IS_LED_ON(usb_led, USB_LED_NUM_LOCK)) {
        writePinLow(B0);
    } else {
        writePinHigh(B0);
    }*/
    if (IS_LED_ON(usb_led, USB_LED_CAPS_LOCK)) {
        writePinLow(CAPSLOCK_PIN);
    } else {
        writePinHigh(CAPSLOCK_PIN);
    }
    /*if (IS_LED_ON(usb_led, USB_LED_SCROLL_LOCK)) {
        writePinLow(B2);
    } else {
        writePinHigh(B2);
    }
    if (IS_LED_ON(usb_led, USB_LED_COMPOSE)) {
        writePinLow(B3);
    } else {
        writePinHigh(B3);
    }
    if (IS_LED_ON(usb_led, USB_LED_KANA)) {
        writePinLow(B4);
    } else {
        writePinHigh(B4);
    }*/
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  return true;
}

/*const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt) {
	keyevent_t event = record->event;

	switch (id) {

	}
	return MACRO_NONE;
}

void matrix_init_user(void) {
}

void matrix_scan_user(void) {
}

void led_set_user(uint8_t usb_led) {

	if (usb_led & (1 << USB_LED_NUM_LOCK)) {
		
	} else {
		
	}

	if (usb_led & (1 << USB_LED_CAPS_LOCK)) {
		DDRF |= (1 << 4); PORTF &= ~(1 << 4);
	} else {
		DDRF &= ~(1 << 4); PORTF &= ~(1 << 4);
	}

	if (usb_led & (1 << USB_LED_SCROLL_LOCK)) {
		
	} else {
		
	}

	if (usb_led & (1 << USB_LED_COMPOSE)) {
		
	} else {
		
	}

	if (usb_led & (1 << USB_LED_KANA)) {
		
	} else {
		
	}

}*/