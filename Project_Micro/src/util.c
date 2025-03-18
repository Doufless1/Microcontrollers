/**
 * Utility functions implementation
 */
#include "../include/util.h"

/**
 * Get absolute value of a 16-bit integer
 * value: Input value
 * Returns: Absolute value
 */
int16_t abs_int16(int16_t value) {
    return value < 0 ? -value : value;
}

/**
 * Convert int16_t to string
 * value: Integer to convert
 * buffer: Buffer to store the string (min size: 7 bytes)
 */
void int_to_str(int16_t value, char* buffer) {
    uint8_t is_negative = 0;
    uint16_t u_value;
    uint8_t i = 0, j = 0;
    char temp_buffer[7];
    
    // Handle negative numbers
    if (value < 0) {
        is_negative = 1;
        u_value = -value;
    } else {
        u_value = value;
    }
    
    // Handle special case of 0
    if (u_value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    
    // Convert digits to ASCII
    while (u_value > 0) {
        temp_buffer[i++] = '0' + (u_value % 10);
        u_value /= 10;
    }
    
    // Add negative sign if needed
    if (is_negative) {
        buffer[j++] = '-';
    }
    
    // Reverse the digits
    while (i > 0) {
        buffer[j++] = temp_buffer[--i];
    }
    
    // Add null terminator
    buffer[j] = '\0';
}
