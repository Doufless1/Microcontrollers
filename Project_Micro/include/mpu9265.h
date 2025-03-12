#ifndef MPU9265_H
#define MPU9265_H

#include <stdint.h>
#include <util/delay.h>
#include "i2c.h"

// MPU-92/65 registers (based on datasheet)
#define MPU9265_ADDR        0x68
#define MPU9265_ADDR_W      ((MPU9265_ADDR << 1) | 0) // Write address
#define MPU9265_ADDR_R      ((MPU9265_ADDR << 1) | 1) // Read address

// Configuration registers
#define MPU9265_PWR_MGMT_1      0x6B
#define MPU9265_PWR_MGMT_2      0x6C
#define MPU9265_SMPLRT_DIV      0x19
#define MPU9265_CONFIG          0x1A
#define MPU9265_GYRO_CONFIG     0x1B
#define MPU9265_ACCEL_CONFIG    0x1C
#define MPU9265_ACCEL_CONFIG2   0x1D
#define MPU9265_INT_PIN_CFG     0x37
#define MPU9265_INT_ENABLE      0x38
#define MPU9265_WHO_AM_I        0x75

// Data registers
#define MPU9265_GYRO_XOUT_H     0x43
#define MPU9265_GYRO_XOUT_L     0x44
#define MPU9265_GYRO_YOUT_H     0x45
#define MPU9265_GYRO_YOUT_L     0x46
#define MPU9265_GYRO_ZOUT_H     0x47
#define MPU9265_GYRO_ZOUT_L     0x48

// Gyroscope configuration values
#define GYRO_FULL_SCALE_250DPS  0x00
#define GYRO_FULL_SCALE_500DPS  0x08
#define GYRO_FULL_SCALE_1000DPS 0x10
#define GYRO_FULL_SCALE_2000DPS 0x18

// Structure to hold gyroscope data
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} gyro_data_t;

// Function declarations
uint8_t mpu9265_init(void);
uint8_t mpu9265_write_reg(uint8_t reg, uint8_t data);
uint8_t mpu9265_read_reg(uint8_t reg);
void mpu9265_read_gyro(gyro_data_t *gyro_data);
uint8_t mpu9265_test_connection(void);

#endif
