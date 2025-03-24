/**
 * UART implementation for AVR
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include "../include/uart.h"

// Circular buffer for receiving data
static char rx_buffer[UART_BUFFER_SIZE];
static volatile int rx_head = 0;
static volatile int rx_tail = 0;

// Flag for command ready
static volatile int command_ready = 0;
static char command_buffer[UART_BUFFER_SIZE];

// Current command being processed
char current_command[UART_BUFFER_SIZE];
int command_param = 0;

/**
 * Initialize UART with specified baud rate
 */
void uart_init(uint32_t baud) {
    // Calculate baud rate register value
    uint16_t baud_setting = (F_CPU / 8 / baud - 1) / 2;
    
    // Set baud rate
    UBRR0H = (int)(baud_setting >> 8);
    UBRR0L = (int)baud_setting;
    
    // Enable receiver, transmitter and receiver interrupt
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    
    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    
    // Enable global interrupts
    sei();
}

/**
 * Send a single character
 */
void uart_putc(char c) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));
    
    // Put data into buffer
    UDR0 = c;
}

/**
 * Send a string
 */
void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

/**
 * Send an integer as string
 */
void uart_puti(int i) {
    char buffer[12]; // Enough for 32-bit int
    itoa(i, buffer, 10);
    uart_puts(buffer);
}

/**
 * Read a character (returns -1 if no data available)
 */
int uart_getc(void) {
    // If buffer is empty, return -1
    if (rx_head == rx_tail) {
        return -1;
    }
    
    // Read from buffer
    int tail = rx_tail;
    char c = rx_buffer[tail];
    rx_tail = (tail + 1) % UART_BUFFER_SIZE;
    
    return c;
}

/**
 * Check if data is available to read
 */
int uart_available(void) {
    return (rx_head != rx_tail);
}

/**
 * Process any received commands
 * Returns: 1 if new command processed, 0 otherwise
 */
int uart_process_command(void) {
    if (!command_ready) {
        return 0;
    }
    
    // Copy command to working buffer
    strcpy(current_command, command_buffer);
    command_ready = 0;
    
    // Find space to separate command and parameter
    char *space = strchr(current_command, ' ');
    if (space != NULL) {
        // Split into command and parameter
        *space = '\0';
        command_param = atoi(space + 1);
    } else {
        command_param = 0;
    }
    
    // Acknowledge command received
    uart_puts("CMD: ");
    uart_puts(current_command);
    uart_puts(" PARAM: ");
    uart_puti(command_param);
    uart_puts("\r\n");
    
    return 1;
}

/**
 * UART Receive Complete interrupt handler
 */
ISR(USART_RX_vect) {
    // Read received byte
    char c = UDR0;
    
    // Echo character back
    uart_putc(c);
    
    // Process character
    if (c == '\r' || c == '\n') {
        // End of command, null-terminate and set flag
        if (rx_head != rx_tail) {
            int i = 0;
            int tail = rx_tail;
            
            // Copy from receive buffer to command buffer
            while (tail != rx_head) {
                command_buffer[i++] = rx_buffer[tail];
                tail = (tail + 1) % UART_BUFFER_SIZE;
            }
            
            // Null-terminate command
            command_buffer[i] = '\0';
            
            // Reset buffer pointers
            rx_head = rx_tail = 0;
            
            // Set command ready flag
            command_ready = 1;
        }
        
        // Echo newline
        uart_putc('\r');
        uart_putc('\n');
    } else {
        // Store character in buffer
        int next_head = (rx_head + 1) % UART_BUFFER_SIZE;
        
        // Discard if buffer full
        if (next_head != rx_tail) {
            rx_buffer[rx_head] = c;
            rx_head = next_head;
        }
    }
}
