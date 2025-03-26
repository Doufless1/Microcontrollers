
#ifndef UART_H
#define UART_H

#include <stdint.h>

// UART buffer size
#define UART_BUFFER_SIZE 64

// Command variables that need to be accessible from main.c
extern char current_command[UART_BUFFER_SIZE];
extern int command_param;

// Initialize UART with specified baud rate
void uart_init(int baud);

// Send a single character
void uart_putc(char c);

// Send a string
void uart_puts(const char *s);

// Send an integer as string
void uart_puti(int i);

// Read a character (returns -1 if no data available)
int uart_getc(void);

// Check if data is available to read
int uart_available(void);

// Process any received commands
int uart_process_command(void);

#endif /* UART_H */
