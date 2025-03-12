#include "../include/serial.h"
#include <avr/io.h>

void serial_init(uint32_t baud) {
    uint16_t ubrr = F_CPU / 16 / baud - 1;
    
    // Set baud rate
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    
    // Enable transmitter
    UCSR0B = (1 << TXEN0);
    
    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void serial_tx_char(char c) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));
    
    // Put data into buffer
    UDR0 = c;
}

void serial_tx_string(const char *str) {
    while (*str) {
        serial_tx_char(*str++);
    }
}

void serial_tx_uint16(uint16_t val) {
    char buffer[6]; // Maximum 5 digits + null terminator
    uint8_t i = 0;
    
    // Convert number to string
    do {
        buffer[i++] = '0' + (val % 10);
        val /= 10;
    } while (val > 0);
    
    // Send digits in reverse order
    while (i > 0) {
        serial_tx_char(buffer[--i]);
    }
}

void serial_tx_int16(int16_t val) {
    if (val < 0) {
        serial_tx_char('-');
        val = -val;
    }
    
    serial_tx_uint16((uint16_t)val);
}
