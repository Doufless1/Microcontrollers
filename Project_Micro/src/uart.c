/**
 * UART implementation for AVR
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include "../include/uart.h"

// we use the volatile because the variables are modifiend during the interup routine
static char rx_buffer[UART_BUFFER_SIZE];
static volatile int rx_head = 0;
static volatile int rx_tail = 0;

static volatile int command_ready = 0;
static char command_buffer[UART_BUFFER_SIZE];

char current_command[UART_BUFFER_SIZE];
int command_param = 0;


void uart_init(int baud) {
  // page 178
    int baud_setting = (F_CPU / 8 / baud - 1) / 2;
    
    // setting up the baud rate
    UBRR0H = (int)(baud_setting >> 8);
    UBRR0L = (int)baud_setting;
   
  // set up reciver transmitter and interupt
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    
    // set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    
    sei();
  // page 241-242
}


void uart_putc(char c) {
    // wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));
    
   // Writing to UDR0 loads the transmit buffer
//The hardware automatically shifts out the data on TXD pin at the configured baud rate
    UDR0 = c;
}


void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

void uart_puti(int i) {
    char buffer[12]; // Enough for 32-bit int
    itoa(i, buffer, 10);
    uart_puts(buffer);
}


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

int uart_available(void) {
    return (rx_head != rx_tail);
}


int uart_process_command(void) {
    if (!command_ready) {
        return 0;
    }
    
    // copy command to working buffer
    strcpy(current_command, command_buffer);
    command_ready = 0;
    
    // find space to separate command and parameter
    char *space = strchr(current_command, ' ');
    if (space != NULL) {
        *space = '\0';
        command_param = atoi(space + 1);
    } else {
        command_param = 0;
    }
    
    uart_puts("CMD: ");
    uart_puts(current_command);
    uart_puts(" PARAM: ");
    uart_puti(command_param);
    uart_puts("\r\n");
    
    return 1;
}

ISR(USART_RX_vect) {
  // Reading UDR0 retrieves the received byte and clears the interrupt flag
    char c = UDR0;
    
    // sends and recives the character back
    uart_putc(c);
    
    if (c == '\r' || c == '\n') { // check for line edingins
        // end of command, null-terminate and set flag
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
