/* Copyright 2022 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

// ToDo: HID for google meet bidirectional feedback

extern MidiDevice midi_device;

/* RME Fireface UCX II MIDI Control: (channel 1 maps to 0...)
Mains Volume: channel 1, CC7, 0 - 127 (0dB @ 104, +6dB @ 127)
Analog 1 Gain: channel 1, CC9, set to desired gain. (AT4040 @ 45db) */
#define AN1_GAIN 45                 // Analog 1 microphone gain in dB
//#define AN2_GAIN 0                  // Analog 2 microphone gain in dB
#define STARTUP_VOLUME 20           // Default UCX II mains volume (0 - 127)
#define VOLUME_STEP 3               // Rotary knob volume change speed

// clang-format off

enum analog_input {
    GAIN_ACTIVE,
    GAIN_MUTE
}analog_1 , analog_2;

enum layers{
    MAC_BASE,
    MAC_FN,
    DVORAK_BASE,
    DVORAK_FN
};

enum custom_keycodes {
    KC_DFU = SAFE_RANGE,
    UCX_AN1_TOG,
    UCX_AN1_PTM,
    UCX_MAINS_VOLU,
    UCX_MAINS_VOLD,
    UCX_MAINS_PTM,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_ansi_89(            // MAC key switch
        UCX_AN1_TOG,   KC_ESC,   KC_BRID,  KC_BRIU,  KC_NO,    KC_NO,    KC_NO,     KC_NO,    KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_INS,             KC_DEL,
        MO(MAC_FN),    KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,      KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_PGUP,
        UCX_AN1_PTM,   KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,      KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,            KC_PGDN,
        UCX_AN1_PTM,   KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,      KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,             KC_HOME,
        UCX_AN1_PTM,   KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,      KC_B,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,  KC_UP,
        UCX_AN1_PTM,   KC_LCTL,  KC_LOPT,            KC_LCMD,  KC_SPC,  MO(MAC_FN),                     KC_SPC,             KC_RCMD,                      KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN] = LAYOUT_ansi_89(              // MAC key switch
        _______,  _______,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,     KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   AC_ON,              AC_OFF,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
        _______,  _______,  _______,  _______,  KC_DFU,   _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,            _______,            _______,
        _______,  _______,            _______,  _______,  _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  RGB_VAI,
        _______,  _______,  _______,            _______,  _______,  _______,                       _______,            _______,                      RGB_MOD,  RGB_VAD,  RGB_TOG),

    [DVORAK_BASE] = LAYOUT_ansi_89(         // WIN key switch
        UCX_AN1_TOG,   KC_ESC,   KC_BRID,  KC_BRIU,  KC_NO,    KC_NO,    KC_NO,     KC_NO,    KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_INS,             KC_DEL,
        MO(DVORAK_FN), KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,      KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_LBRC,  KC_RBRC,  KC_BSPC,            KC_PGUP,
        UCX_AN1_PTM,   KC_TAB,   KC_QUOT,  KC_COMM,  KC_DOT,   KC_P,     KC_Y,      KC_F,     KC_G,     KC_C,     KC_R,     KC_L,     KC_SLSH,  KC_EQL,   KC_BSLS,            KC_PGDN,
        UCX_AN1_PTM,   KC_CAPS,  KC_A,     KC_O,     KC_E,     KC_U,     KC_I,      KC_D,     KC_H,     KC_T,     KC_N,     KC_S,     KC_MINS,            KC_ENT,             KC_HOME,
        UCX_AN1_PTM,   KC_LSFT,            KC_SCLN,  KC_Q,     KC_J,     KC_K,      KC_X,     KC_X,     KC_B,     KC_M,     KC_W,     KC_V,     KC_Z,     KC_RSFT,  KC_UP,
        UCX_AN1_PTM,   KC_LCTL,  KC_LOPT,            KC_LCMD,  KC_SPC,  MO(DVORAK_FN),                  KC_SPC,             MO(DVORAK_FN),                KC_LEFT,  KC_DOWN,  KC_RGHT),

    [DVORAK_FN] = LAYOUT_ansi_89(           // WIN key switch
        _______,  _______,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,     KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,    AC_ON,              AC_OFF,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,            _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,   KC_7,     KC_8,     KC_9,     KC_PPLS,  _______,  _______,   _______,  _______,            _______,
        _______,  _______,  _______,  _______,  KC_DFU,   _______,  _______,   KC_4,     KC_5,     KC_6,     KC_PPLS,  _______,  _______,             _______,            _______,
        _______,  _______,            _______,  _______,  _______,  _______,   _______,  KC_PPLS,  KC_1,     KC_2,     KC_3,     KC_PENT,   _______,  _______,  RGB_VAI,
        _______,  _______,  _______,            _______,  _______,  _______,                       KC_0,               KC_PDOT,                       RGB_MOD,  RGB_VAD,  RGB_TOG),
};

#if defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [MAC_BASE] = { ENCODER_CCW_CW(UCX_MAINS_VOLD, UCX_MAINS_VOLU) },
    [MAC_FN]   = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [DVORAK_BASE] = { ENCODER_CCW_CW(UCX_MAINS_VOLD, UCX_MAINS_VOLU) },
    [DVORAK_FN]   = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) }
};
#endif

void keyboard_post_init_user(void) {
  // Call the post init code.
  analog_1 = GAIN_MUTE;
  analog_2 = GAIN_MUTE;
  midi_send_cc(&midi_device, 0 , 0x07, STARTUP_VOLUME); // Set Mains to startup volume
  midi_send_cc(&midi_device, 0 , 0x09, 0); // UCX_AN1 MUTE
  rgblight_disable();
}

void suspend_power_down_user(void) {
    // code will run multiple times while keyboard is suspended
    keyboard_post_init_user();
}

// clang-format on
static uint8_t mains_volume = STARTUP_VOLUME;
static uint32_t key_timer;
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_DFU:                                // Reboot for firmware flashing
            if (record->event.pressed) {
                key_timer = timer_read32();
            } else {
                if (timer_elapsed32(key_timer) >= 500) {
                    reset_keyboard();
                }
            }
            return false;
        case UCX_AN1_PTM:                           // Analog 1 push to mute AND push to talk.
            if (analog_1 == GAIN_ACTIVE) {
                midi_send_cc(&midi_device, 0 , 0x09, 0);
                analog_1 = GAIN_MUTE;
                rgblight_disable();
            } else {
                midi_send_cc(&midi_device, 0 , 0x09, AN1_GAIN);
                analog_1 = GAIN_ACTIVE;
                rgblight_enable();
            }
            return false;
        case UCX_AN1_TOG:                           // Analog 1 digital mute toggle
            if (record->event.pressed) {
                if (analog_1){
                    midi_send_cc(&midi_device, 0 , 0x09, AN1_GAIN);
                    rgblight_enable();
                }
                else {                              // Apply -45dB pad instead of toggling 48V phantom for zero pop on condenser mics
                    midi_send_cc(&midi_device, 0 , 0x09, 0);
                    rgblight_disable();
                }
                analog_1 = !analog_1;
            }
            return false;
        case UCX_MAINS_VOLU:                           // Mains Volume Up
            if (record->event.pressed) {
                if (mains_volume + VOLUME_STEP <= 104) {
                    mains_volume += VOLUME_STEP;
                }
                else {
                    mains_volume = 104;
                }
                midi_send_cc(&midi_device, 0 , 0x07, mains_volume);
            }
            return false;
        case UCX_MAINS_VOLD:                           // Mains Volume Down
            if (record->event.pressed) {
               if (mains_volume - VOLUME_STEP >= 0) {
                    mains_volume -= VOLUME_STEP;
                }
                else {
                    mains_volume = 0;
                }
                midi_send_cc(&midi_device, 0 , 0x07, mains_volume);
            }
            return false;
        case UCX_MAINS_PTM:                         // Mains volume push to mute
            if (record->event.pressed) {
                midi_send_cc(&midi_device, 0 , 0x07, 0);
            } else {
                midi_send_cc(&midi_device, 0 , 0x07, mains_volume);
            }
            return false;
        default:
            return true;
    }
}
