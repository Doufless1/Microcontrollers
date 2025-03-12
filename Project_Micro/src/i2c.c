#include "../include/i2c.h"
void i2c_init(void) {
    // Set SCL frequency to 100KHz with 16MHz system clock
    // SCL frequency = CPU clock / (16 + 2 * TWBR * prescaler)
    TWBR = 72;  // Prescaler = 1, TWBR = 72 gives 100KHz
    
    // Enable TWI module
    TWCR = (1 << TWEN);
}

uint8_t i2c_start(uint8_t address) {
    // Send START condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    
    // Wait for TWINT flag to be set
    while (!(TWCR & (1 << TWINT)));
    
    // Check if START was successfully transmitted
    if ((TWSR & 0xF8) != I2C_START && (TWSR & 0xF8) != I2C_REPEATED_START) {
        return 0;
    }
    
    // Load SLA+W/R into TWDR register
    TWDR = address;
    
    // Clear TWINT flag to start transmission
    TWCR = (1 << TWINT) | (1 << TWEN);
    
    // Wait for TWINT flag to be set
    while (!(TWCR & (1 << TWINT)));
    
    // Check if SLA+W/R was successfully transmitted and ACK received
    if ((TWSR & 0xF8) != I2C_MT_SLA_ACK && (TWSR & 0xF8) != I2C_MR_SLA_ACK) {
        return 0;
    }
    
    return 1;
}

uint8_t i2c_write(uint8_t data) {
    // Load data into TWDR register
    TWDR = data;
    
    // Clear TWINT flag to start transmission
    TWCR = (1 << TWINT) | (1 << TWEN);
    
    // Wait for TWINT flag to be set
    while (!(TWCR & (1 << TWINT)));
    
    // Check if data was successfully transmitted and ACK received
    if ((TWSR & 0xF8) != I2C_MT_DATA_ACK) {
        return 0;
    }
    
    return 1;
}

uint8_t i2c_read_ack(void) {
    // Clear TWINT flag and enable ACK
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    
    // Wait for TWINT flag to be set
    while (!(TWCR & (1 << TWINT)));
    
    // Return received data
    return TWDR;
}

uint8_t i2c_read_nack(void) {
    // Clear TWINT flag
    TWCR = (1 << TWINT) | (1 << TWEN);
    
    // Wait for TWINT flag to be set
    while (!(TWCR & (1 << TWINT)));
    
    // Return received data
    return TWDR;
}

void i2c_stop(void) {
    // Send STOP condition
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    
    // Wait for STOP condition to be executed
    while (TWCR & (1 << TWSTO));
}
