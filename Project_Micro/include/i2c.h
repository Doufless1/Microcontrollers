#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

// I2C status codes
#define I2C_START           0x08
#define I2C_REPEATED_START  0x10
#define I2C_MT_SLA_ACK      0x18
#define I2C_MT_SLA_NACK     0x20
#define I2C_MT_DATA_ACK     0x28
#define I2C_MR_SLA_ACK      0x40
#define I2C_MR_SLA_NACK     0x48
#define I2C_MR_DATA_ACK     0x50
#define I2C_MR_DATA_NACK    0x58

// Function declarations
void i2c_init(void);
uint8_t i2c_start(uint8_t address);
uint8_t i2c_write(uint8_t data);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);
void i2c_stop(void);

#endif
