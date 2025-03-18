#include "../include/mpu6050.h"
#include "../include/i2c.h"
#include <util/delay.h>

// I2C addresses
uint8_t MPU6050_ADDR_W = (MPU6050_ADDR << 1);
uint8_t MPU6050_ADDR_R = (MPU6050_ADDR << 1) | 1;

/**
 * Read a register from MPU6050
 */
uint8_t mpu6050_read_reg(uint8_t reg) {
    i2c_start(MPU6050_ADDR_W);
    i2c_write(reg);
    i2c_start(MPU6050_ADDR_R);
    uint8_t data = i2c_read_nack();
    i2c_stop();
    return data;
}

/**
 * Write to a register on MPU6050
 */
void mpu6050_write_reg(uint8_t reg, uint8_t value) {
    i2c_start(MPU6050_ADDR_W);
    i2c_write(reg);
    i2c_write(value);
    i2c_stop();
}

/**
 * Test connection to MPU6050
 * Returns 1 if successful, 0 if failed
 */
uint8_t mpu6050_test_connection(void) {
    uint8_t who_am_i = mpu6050_read_reg(MPU6050_WHO_AM_I);
    
    // MPU6050 should return 0x68 for WHO_AM_I
    return (who_am_i == 0x68);
}

/**
 * Initialize MPU6050
 * Returns 0 if successful, 1 if failed
 */
uint8_t mpu6050_init(void) {
    // Delay for device power-up
    _delay_ms(100);
    
    // Check if device is responding
    if (!mpu6050_test_connection()) {
        // Try alternate address (if AD0 pin is high)
        MPU6050_ADDR_W = ((0x69 << 1) | 0);
        MPU6050_ADDR_R = ((0x69 << 1) | 1);
        
        // Check again
        if (!mpu6050_test_connection()) {
            // Reset to default address
            MPU6050_ADDR_W = (MPU6050_ADDR << 1);
            MPU6050_ADDR_R = (MPU6050_ADDR << 1) | 1;
            return 1;  // Error: device not found
        }
    }
    
    // Reset the device
    mpu6050_write_reg(MPU6050_PWR_MGMT_1, 0x80);  // Reset bit
    _delay_ms(100);
    
    // Wake up the sensor (clear sleep bit)
    mpu6050_write_reg(MPU6050_PWR_MGMT_1, 0x00);
    _delay_ms(10);
    
    // PLL with X axis gyroscope as clock reference
    mpu6050_write_reg(MPU6050_PWR_MGMT_1, 0x01);
    _delay_ms(10);
    
    // Configure gyro for ±250 deg/s range (for best sensitivity)
    mpu6050_write_reg(MPU6050_GYRO_CONFIG, 0x00);
    
    // Configure accelerometer for ±2g range (for best sensitivity)
    mpu6050_write_reg(MPU6050_ACCEL_CONFIG, 0x00);
    
    // Set sample rate divider
    mpu6050_write_reg(MPU6050_SMPLRT_DIV, 0x07);  // 1kHz/(1+7) = 125Hz sample rate
    
    // Configure digital low pass filter
    mpu6050_write_reg(MPU6050_CONFIG, 0x06);  // ~5Hz bandwidth for both accel and gyro
    
    return 0;  // Success
}

/**
 * Read gyroscope data
 */
void mpu6050_read_gyro(gyro_data_t *data) {
    data->x = ((int16_t)mpu6050_read_reg(MPU6050_GYRO_XOUT_H) << 8) | mpu6050_read_reg(MPU6050_GYRO_XOUT_L);
    data->y = ((int16_t)mpu6050_read_reg(MPU6050_GYRO_YOUT_H) << 8) | mpu6050_read_reg(MPU6050_GYRO_YOUT_L);
    data->z = ((int16_t)mpu6050_read_reg(MPU6050_GYRO_ZOUT_H) << 8) | mpu6050_read_reg(MPU6050_GYRO_ZOUT_L);
}

/**
 * Read accelerometer data
 */
void mpu6050_read_accel(accel_data_t *data) {
    data->x = ((int16_t)mpu6050_read_reg(MPU6050_ACCEL_XOUT_H) << 8) | mpu6050_read_reg(MPU6050_ACCEL_XOUT_L);
    data->y = ((int16_t)mpu6050_read_reg(MPU6050_ACCEL_YOUT_H) << 8) | mpu6050_read_reg(MPU6050_ACCEL_YOUT_L);
    data->z = ((int16_t)mpu6050_read_reg(MPU6050_ACCEL_ZOUT_H) << 8) | mpu6050_read_reg(MPU6050_ACCEL_ZOUT_L);
}
