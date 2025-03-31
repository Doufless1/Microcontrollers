
#ifndef UART_H
#define UART_H

#include <stdint.h>

#define UART_BUFFER_SIZE 64

extern char current_command[UART_BUFFER_SIZE];
extern int command_param;

void uart_init(int baud);

void uart_putc(char c);

// Send a string
void uart_puts(const char *s);

// send an integer as string
void uart_puti(int i);

int uart_getc(void);

// check if data is available to read
int uart_available(void);

// process any received commands
int uart_process_command(void);

#endif /* UART_H */
