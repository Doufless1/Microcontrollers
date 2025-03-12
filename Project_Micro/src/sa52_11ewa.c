#include "../include/sa52_11ewa.h"
#include <stdlib.h>

// Segment patterns for digits 0-9
// For common cathode display, 1=on, 0=off
const uint8_t digit_patterns[10] = {
    // a, b, c, d, e, f, g
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F,           // 0
    SEGMENT_B | SEGMENT_C,                                                           // 1
    SEGMENT_A | SEGMENT_B | SEGMENT_G | SEGMENT_E | SEGMENT_D,                       // 2
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,                       // 3
    SEGMENT_F | SEGMENT_G | SEGMENT_B | SEGMENT_C,                                   // 4
    SEGMENT_A | SEGMENT_F | SEGMENT_G | SEGMENT_C | SEGMENT_D,                       // 5
    SEGMENT_A | SEGMENT_F | SEGMENT_E | SEGMENT_D | SEGMENT_C | SEGMENT_G,           // 6
    SEGMENT_A | SEGMENT_B | SEGMENT_C,                                               // 7
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G, // 8
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G            // 9
};

// Pattern for minus sign
#define MINUS_PATTERN SEGMENT_G

void sa52_11ewa_init(void) {
    // Initialize PCF8574
    pcf8574_init();
    
    // Clear display
    sa52_11ewa_clear();
}

void sa52_11ewa_display_digit(uint8_t digit) {
    if (digit > 9) {
        sa52_11ewa_clear();
        return;
    }
    
    // For common cathode, we send the pattern directly
    pcf8574_write(digit_patterns[digit]);
}

void sa52_11ewa_display_minus(void) {
    pcf8574_write(MINUS_PATTERN);
}

void sa52_11ewa_clear(void) {
    // Turn off all segments
    pcf8574_write(0x00);
}

void sa52_11ewa_display_number(int16_t number) {
    // Special case for zero
    if (number == 0) {
        sa52_11ewa_display_digit(0);
        return;
    }
    
    // Handle negative numbers
    if (number < 0) {
        sa52_11ewa_display_minus();
        _delay_ms(1000);
        number = -number;
    }
    
    // Display each digit with delay
    uint16_t num = (uint16_t)number;
    uint8_t digit_count = 0;
    uint16_t temp = num;
    
    // Count number of digits
    while (temp > 0) {
        temp /= 10;
        digit_count++;
    }
    
    // Extract and display each digit
    for (uint8_t i = 0; i < digit_count; i++) {
        uint8_t digit = num % 10;
        sa52_11ewa_display_digit(digit);
        _delay_ms(1000); // Display each digit for 1 second
        num /= 10;
    }
}
