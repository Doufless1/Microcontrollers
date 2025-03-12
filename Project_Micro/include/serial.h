#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <avr/io.h>

// Function declarations
void serial_init(uint32_t baud);
void serial_tx_char(char c);
void serial_tx_string(const char *str);
void serial_tx_uint16(uint16_t val);
void serial_tx_int16(int16_t val);

#endif
