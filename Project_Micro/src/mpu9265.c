#include "../include/mpu9265.h"

uint8_t mpu9265_init(void) {
    uint8_t check;
    
    // Reset device - write 0x80 to PWR_MGMT_1 register
    mpu9265_write_reg(MPU9265_PWR_MGMT_1, 0x80);
    _delay_ms(100); // Wait for reset to complete
    
    // Check device ID
    check = mpu9265_read_reg(MPU9265_WHO_AM_I);
    
    // MPU-9250/9265 should return 0x71 or 0x73 as WHO_AM_I
    if (check != 0x71 && check != 0x73) {
        return 0; // Device not found or wrong device
    }
    
    // Wake up the MPU-9265 (clear sleep bit)
    mpu9265_write_reg(MPU9265_PWR_MGMT_1, 0x01); // Use PLL with X axis gyro reference
    
    // Set sample rate = gyroscope output rate / (1 + SMPLRT_DIV)
    mpu9265_write_reg(MPU9265_SMPLRT_DIV, 0x07); // 1kHz / (1 + 7) = 125Hz
    
    // Configure gyroscope range
    mpu9265_write_reg(MPU9265_GYRO_CONFIG, GYRO_FULL_SCALE_250DPS); // ±250 degrees/second
    
    // Configure accelerometer range (not used in this project but good for initialization)
    mpu9265_write_reg(MPU9265_ACCEL_CONFIG, 0x00); // ±2g
    
    // Configure digital low pass filter
    mpu9265_write_reg(MPU9265_CONFIG, 0x03); // ~43Hz bandwidth
    
    return 1;
}

uint8_t mpu9265_write_reg(uint8_t reg, uint8_t data) {
    if (!i2c_start(MPU9265_ADDR_W)) {
        return 0;
    }
    
    if (!i2c_write(reg)) {
        return 0;
    }
    
    if (!i2c_write(data)) {
        return 0;
    }
    
    i2c_stop();
    return 1;
}

uint8_t mpu9265_read_reg(uint8_t reg) {
    uint8_t data;
    
    if (!i2c_start(MPU9265_ADDR_W)) {
        return 0;
    }
    
    if (!i2c_write(reg)) {
        return 0;
    }
    
    if (!i2c_start(MPU9265_ADDR_R)) {
        return 0;
    }
    
    data = i2c_read_nack();
    
    i2c_stop();
    
    return data;
}

void mpu9265_read_gyro(gyro_data_t *gyro_data) {
    uint8_t xh, xl, yh, yl, zh, zl;
    
    // Start at register GYRO_XOUT_H
    i2c_start(MPU9265_ADDR_W);
    i2c_write(MPU9265_GYRO_XOUT_H);
    
    // Read 6 bytes (X, Y, Z high and low bytes)
    i2c_start(MPU9265_ADDR_R);
    xh = i2c_read_ack();
    xl = i2c_read_ack();
    yh = i2c_read_ack();
    yl = i2c_read_ack();
    zh = i2c_read_ack();
    zl = i2c_read_nack();
    
    i2c_stop();
    
    // Combine high and low bytes into 16-bit values
    gyro_data->x = (int16_t)((xh << 8) | xl);
    gyro_data->y = (int16_t)((yh << 8) | yl);
    gyro_data->z = (int16_t)((zh << 8) | zl);
}

uint8_t mpu9265_test_connection(void) {
    uint8_t who_am_i = mpu9265_read_reg(MPU9265_WHO_AM_I);
    return (who_am_i == 0x71 || who_am_i == 0x73);
}
