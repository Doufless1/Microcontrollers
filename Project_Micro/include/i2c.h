/**
 * I2C communication interface for AVR
 */
#ifndef I2C_H
#define I2C_H

#include <stdint.h>

/* I2C status codes */
#define I2C_START             0x08
#define I2C_REPEATED_START    0x10
#define I2C_SLA_W_ACK         0x18
#define I2C_SLA_W_NACK        0x20
#define I2C_DATA_ACK          0x28
#define I2C_DATA_NACK         0x30
#define I2C_SLA_R_ACK         0x40
#define I2C_SLA_R_NACK        0x48
#define I2C_DATA_RX_ACK       0x50
#define I2C_DATA_RX_NACK      0x58

/* Function prototypes */
void i2c_init(void);
uint8_t i2c_start(uint8_t address);
void i2c_start_wait(uint8_t address);
uint8_t i2c_write(uint8_t data);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);
void i2c_stop(void);

#endif /* I2C_H */
