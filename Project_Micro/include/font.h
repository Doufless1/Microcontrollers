/**
 * 5x7 font for OLED display
 */
#ifndef FONT_H
#define FONT_H

#include <avr/pgmspace.h>

// Font table: 5x7 font
// Each character is 5 bytes wide
// @explain This function decrease a 2d array that stores the piel paterns for each ASCII character so we can write them on the display. Each character represent 5 bits of coliumns and 8 bits of heihgt but only 7 are used
// We used the PROGMEM keyword store the front data in a flash memory rather than a RAM which helps with the limited Ram memory microcontrollers have. btw if we remove the the PROGMEM we have a bout 475 bytes of font data which would go to the SRAM memory during our setup and unfortunatly we are have only 2KB of SRAM which would go for nothing. Even tho using SRAM to access the data would be a bit faster but we would run maybe to stack collisions in your heap data which could also lead to strange behaviours like freezing and reseting.
extern const int font5x7[][5] PROGMEM;

// Lastly we used the extern to avoid the multiple definitiong error


#endif /* FONT_H */
