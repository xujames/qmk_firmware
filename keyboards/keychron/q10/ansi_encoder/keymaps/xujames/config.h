#pragma once

#define MIDI_ADVANCED
#define RGB_DISABLE_TIMEOUT 600000      // number of milliseconds to wait until rgb automatically turns off
#define RGB_DISABLE_WHEN_USB_SUSPENDED  // turn off effects when suspended
#define USB_POLLING_INTERVAL_MS 1

#undef DEBOUNCE
#define DEBOUNCE 10

//#define DEBUG_MATRIX_SCAN_RATE
