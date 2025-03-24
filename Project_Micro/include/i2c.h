#ifndef I2C_H
#define I2C_H

#include <stdint.h>

/* I2C status codes */
#define I2C_START             0x08 // The START condition was transmitted successfully
#define I2C_REPEATED_START    0x10 // A repeated START condition was transmitted
#define I2C_SLA_W_ACK         0x18 // slave address + the write was transimitted and ACK recived it.
#define I2C_SLA_W_NACK        0x20 // slave address + the write was transmitted but NACK recived it.
#define I2C_DATA_ACK          0x28 // Data byte was transmitted and ACK recived it.
#define I2C_DATA_NACK         0x30 // Data byte was transmitted but NACK recived it.
#define I2C_SLA_R_ACK         0x40 // slave address and read was transimitted and ACK recived 
#define I2C_SLA_R_NACK        0x48 // slave adress and read was transmitted but NACK recived it.
#define I2C_DATA_RX_ACK       0x50 // Data byte was recived and ACK was returned.
#define I2C_DATA_RX_NACK      0x58 // Data byte was recived and NACK was returned.

// You can see all of this at the ATmega328P on page 186.

void i2c_init(void);
int i2c_start(int address);
void i2c_start_wait(int address);
int i2c_write(int data);
int i2c_read_ack(void);
int i2c_read_nack(void);
void i2c_stop(void);

#endif 
