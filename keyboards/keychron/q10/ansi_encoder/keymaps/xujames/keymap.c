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
#include "features/socd_cleaner.h"

socd_cleaner_t socd_v = {{KC_W, KC_S}, SOCD_CLEANER_LAST};
socd_cleaner_t socd_h = {{KC_A, KC_D}, SOCD_CLEANER_LAST};

extern MidiDevice midi_device;

#define IDLE_TIMEOUT_MS 3600000     // Idle timeout in milliseconds.
/* RME Fireface UCX II MIDI Control: (channel 1 maps to 0...)
Mains Volume: channel 1, CC7, 0 - 127 (0dB @ 104, +6dB @ 127)
Analog 1 Gain: channel 1, CC9, set to desired gain. (AT4040 @ 45db) */
#define AN1_GAIN 45                 // Analog 1 microphone gain in dB
#define AN2_GAIN 0                  // Analog 2 microphone gain in dB
#define STARTUP_VOLUME 20           // Default UCX II mains volume (0 - 127)
#define VOLUME_STEP 2               // Rotary knob volume change speed
#define VOLUME_LIMIT 104            // Volume limit (-13.8db @ 60)

#define CLAMP(v, min, max) ((v) < (min) ? (min) : ((v) > (max) ? (max) : (v)))

static uint8_t mains_volume = STARTUP_VOLUME;

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
    SOCDTOG,
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
        _______,  _______,            _______,  _______,  _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  RM_VALU,
        SOCDTOG,  _______,  _______,            _______,  _______,  _______,                       _______,            _______,                      RM_NEXT,  RM_VALD,  RM_TOGG),

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
        _______,  _______,            _______,  _______,  _______,  _______,   _______,  KC_PPLS,  KC_1,     KC_2,     KC_3,     KC_PENT,   _______,  _______,  RM_VALU,
        SOCDTOG,  _______,  _______,            _______,  _______,  _______,                       KC_0,               KC_PDOT,                       RM_NEXT,  RM_VALD,  RM_TOGG),
};

#if defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [MAC_BASE] = { ENCODER_CCW_CW(UCX_MAINS_VOLD, UCX_MAINS_VOLU) },
    [MAC_FN]   = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [DVORAK_BASE] = { ENCODER_CCW_CW(UCX_MAINS_VOLD, UCX_MAINS_VOLU) },
    [DVORAK_FN]   = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) }
};
#endif

void UCX_cleanup(void) {
    analog_1 = GAIN_MUTE;
    analog_2 = GAIN_MUTE;
    midi_send_cc(&midi_device, 0, 0x07, STARTUP_VOLUME); // Set Mains to startup volume
    midi_send_cc(&midi_device, 0, 0x09, 0); // UCX_AN1 MUTE
    rgb_matrix_disable_noeeprom();
}

static uint32_t idle_callback(uint32_t trigger_time, void* cb_arg) {
    // If execution reaches here, the keyboard has gone idle.
    UCX_cleanup();
    return 0;
}

void keyboard_post_init_user(void) {
    // Call the post init code.
    wait_ms(2000);      //wait for OS to recognize the device
    UCX_cleanup();
}

void adjust_mains_volume(int8_t volume_change) {
    mains_volume = CLAMP(mains_volume + volume_change, 0, VOLUME_LIMIT);
    midi_send_cc(&midi_device, 0, 0x07, mains_volume);
}

// clang-format on
static uint32_t key_timer;
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_socd_cleaner(keycode, record, &socd_v)) { return false; }
    if (!process_socd_cleaner(keycode, record, &socd_h)) { return false; }

    static deferred_token idle_token = INVALID_DEFERRED_TOKEN;
    if (!extend_deferred_exec(idle_token, IDLE_TIMEOUT_MS)) {
        idle_token = defer_exec(IDLE_TIMEOUT_MS, idle_callback, NULL);
    }

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
                analog_1 = !analog_1;
                rgb_matrix_disable_noeeprom();
            } else {
                midi_send_cc(&midi_device, 0 , 0x09, AN1_GAIN);
                analog_1 = !analog_1;
                rgb_matrix_enable_noeeprom();
            }
            return false;
        case UCX_AN1_TOG:                           // Analog 1 digital mute toggle
            if (record->event.pressed) {
                if (analog_1){
                    midi_send_cc(&midi_device, 0 , 0x09, AN1_GAIN);
                    rgb_matrix_enable_noeeprom();
                }
                else {                              // Apply -45dB pad instead of toggling 48V phantom for zero pop on condenser mics
                    midi_send_cc(&midi_device, 0 , 0x09, 0);
                    rgb_matrix_disable_noeeprom();
                }
                analog_1 = !analog_1;
            }
            return false;
        case UCX_MAINS_VOLU:                        // Mains Volume Up
            if (record->event.pressed) {
                adjust_mains_volume(VOLUME_STEP);
            }
            return false;
        case UCX_MAINS_VOLD:                        // Mains Volume Down
            if (record->event.pressed) {
                adjust_mains_volume(-VOLUME_STEP);
            }
            return false;
        case UCX_MAINS_PTM:                         // Mains volume push to mute
            if (record->event.pressed) {
                midi_send_cc(&midi_device, 0 , 0x07, 0);
            } else {
                midi_send_cc(&midi_device, 0 , 0x07, mains_volume);
            }
            return false;
        case SOCDTOG:                              // SOCD Cleaner toggle
            if (record->event.pressed) {
                socd_cleaner_enabled = !socd_cleaner_enabled;
            }
            return false;
        default:
            return true;
    }
}

static bool fn_layer_rgb_was_off = false;

layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);

    // When entering FN layer, enable RGB if needed
    if (layer == MAC_FN || layer == DVORAK_FN) {
        if (!rgb_matrix_is_enabled()) {
            fn_layer_rgb_was_off = true;
            rgb_matrix_enable_noeeprom();
        }
    } else {
        // When leaving FN layer, restore RGB state
        if (fn_layer_rgb_was_off && rgb_matrix_is_enabled()) {
            rgb_matrix_disable_noeeprom();
            fn_layer_rgb_was_off = false;
        }
    }

    return state;
}

bool rgb_matrix_indicators_user(void) {
    // Show SOCD indicator on WASD when FN is held
    uint8_t layer = get_highest_layer(layer_state);

    if (layer == MAC_FN || layer == DVORAK_FN) {
        // Set WASD colors based on SOCD state
        uint8_t r = socd_cleaner_enabled ? 0 : 255;
        uint8_t g = socd_cleaner_enabled ? 255 : 0;
        uint8_t b = 0;

        // Light up WASD keys
        rgb_matrix_set_color(34, r, g, b);  // W
        rgb_matrix_set_color(49, r, g, b);  // A
        rgb_matrix_set_color(50, r, g, b);  // S
        rgb_matrix_set_color(51, r, g, b);  // D
    }

    return false;
}
