/**
 * @file .
 * @brief External source inclusion.
 *
 * This is merely a wrapper to the actual source code.
 * This allows to use the source code from a different location within this
 * project.
 */

#include <Arduino.h>

/*
 * INO files are usually used within the Arduino IDE.
 * The Arduino IDE implicitly includes <Arduino.h>.
 * For compatibility that header is included explicitly before.
 */
#include "../../esp32_usb_parallelport.ino"
