/**
 * 5x7 font for OLED display
 */
#ifndef FONT_H
#define FONT_H

#include <avr/pgmspace.h>

// Font table: 5x7 font
// Each character is 5 bytes wide
extern const uint8_t font5x7[][5] PROGMEM;

#endif /* FONT_H */
