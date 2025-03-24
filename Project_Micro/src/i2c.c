/**
 * I2C communication implementation for AVR
 */
#include <avr/io.h>
#include "../include/i2c.h"


/**
 * Initialize I2C communication with standard 100kHz clock
 */
void i2c_init(void) {
    // Set SCL frequency to 100kHz for 16MHz CPU clock
    TWSR = 0;                             // No prescaler
    TWBR = (F_CPU / 100000UL - 16) / 2;   // Set bit rate register
    TWCR = (1 << TWEN);                   // Enable I2C interface
}

/**
 * Send I2C start condition and slave address
 * address: Slave address with read/write bit
 * Returns: 0 if successful, 1 if error
 */
int i2c_start(int address) {
    // Send START condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    
    // Wait for TWINT flag to be set
    while (!(TWCR & (1 << TWINT)));
    
    // Check if START was sent successfully
    if ((TWSR & 0xF8) != I2C_START && (TWSR & 0xF8) != I2C_REPEATED_START) {
        return 1;
    }
    
    // Load address into data register
    TWDR = address;
    TWCR = (1 << TWINT) | (1 << TWEN);
    
    // Wait for TWINT flag to be set
    while (!(TWCR & (1 << TWINT)));
    
    // Check if address was acknowledged
    if ((TWSR & 0xF8) != I2C_SLA_W_ACK && (TWSR & 0xF8) != I2C_SLA_R_ACK) {
        i2c_stop();
        return 1;
    }
    
    return 0;
}

/**
 * Repeatedly try to start I2C transmission until successful (for device detection)
 * address: Slave address with read/write bit
 */
void i2c_start_wait(int address) {
    int status;
    do {
        // Send START condition
        TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
        
        // Wait for TWINT flag to be set
        while (!(TWCR & (1 << TWINT)));
        
        status = TWSR & 0xF8;
        if (status != I2C_START && status != I2C_REPEATED_START) continue;
        
        // Load address into data register
        TWDR = address;
        TWCR = (1 << TWINT) | (1 << TWEN);
        
        // Wait for TWINT flag to be set
        while (!(TWCR & (1 << TWINT)));
        
        status = TWSR & 0xF8;
    } while (status != I2C_SLA_W_ACK && status != I2C_SLA_R_ACK);
}

/**
 * Write data byte to I2C bus
 * data: Byte to send
 * Returns: 0 if successful, 1 if error
 */
int i2c_write(int data) {
    // Load data into register
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    
    // Wait for TWINT flag to be set
    while (!(TWCR & (1 << TWINT)));
    
    // Check if data was acknowledged
    if ((TWSR & 0xF8) != I2C_DATA_ACK) {
        return 1;
    }
    
    return 0;
}

/**
 * Read data byte from I2C bus and send ACK
 * Returns: Received data byte
 */
int i2c_read_ack(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

/**
 * Read data byte from I2C bus and send NACK
 * Returns: Received data byte
 */
int i2c_read_nack(void) {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

/**
 * Send I2C stop condition
 */
void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while (TWCR & (1 << TWSTO));  // Wait until STOP is cleared
}
