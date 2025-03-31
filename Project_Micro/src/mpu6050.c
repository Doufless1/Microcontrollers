#include "../include/mpu6050.h"
#include "../include/i2c.h"
#include <util/delay.h>

int MPU6050_ADDR_W = (MPU6050_ADDR << 1);
int MPU6050_ADDR_R = (MPU6050_ADDR << 1) | 1;

// page 35-36 MPU6050
// The I²C read involves first writing the register address, then reading data
//This follows the standard I²C protocol described in the datasheet with START, device address, register address, repeated START, data transfer, and STOP conditions
int mpu6050_read_reg(int reg) {
    i2c_start(MPU6050_ADDR_W);
    i2c_write(reg);
    i2c_start(MPU6050_ADDR_R);
    int data = i2c_read_nack();
    i2c_stop();
    return data;
}


void mpu6050_write_reg(int reg, int value) {
    i2c_start(MPU6050_ADDR_W);
    i2c_write(reg);
    i2c_write(value);
    i2c_stop();
}


//WHO_AM_I register (0x75) contains the 6-bit I2C address of the MPU-60X0 which should be 0x68
int mpu6050_test_connection(void) {
    int who_am_i = mpu6050_read_reg(MPU6050_WHO_AM_I);
    
    // MPU6050 should return 0x68 for WHO_AM_I
    return (who_am_i == 0x68);
}


int mpu6050_init(void) {
    _delay_ms(100);
    
    // check if device is responding
    if (!mpu6050_test_connection()) {
        // try alternate address (if AD0 pin is high)
        MPU6050_ADDR_W = ((0x69 << 1) | 0);
        MPU6050_ADDR_R = ((0x69 << 1) | 1);
        
        if (!mpu6050_test_connection()) {
            MPU6050_ADDR_W = (MPU6050_ADDR << 1);
            MPU6050_ADDR_R = (MPU6050_ADDR << 1) | 1;
        }
    }
    
  /* Bit 7 (0x80): Device reset - resets all internal registers to their default values
Bit 6 (0x40): Sleep mode control (0 = normal operation)
Bits 2:0 (0x01): Clock source (1 = PLL with X axis gyroscope reference)
*/
    _delay_ms(100);
   /*
    Bits 4:3 (FS_SEL) selects the full scale range of the gyroscope:
0x00 = ±250°/s
0x08 = ±500°/s
0x10 = ±1000°/s
0x18 = ±2000°/s*/
    // wake up the sensor (clear sleep bit)
    mpu6050_write_reg(MPU6050_PWR_MGMT_1, 0x00);
    _delay_ms(10);
    
    // PLL with X axis gyroscope as clock reference
    mpu6050_write_reg(MPU6050_PWR_MGMT_1, 0x01);
    _delay_ms(10);
    
    mpu6050_write_reg(MPU6050_GYRO_CONFIG, 0x00);
    
    mpu6050_write_reg(MPU6050_ACCEL_CONFIG, 0x00);
    
    mpu6050_write_reg(MPU6050_SMPLRT_DIV, 0x07);  // 1kHz/(1+7) = 125Hz sample rate
    
    // Configure digital low pass filter
    mpu6050_write_reg(MPU6050_CONFIG, 0x06);  // ~5Hz bandwidth for both accel and gyro
    
    return 0;  
}


void mpu6050_read_gyro(gyro_data_t *data) {
    data->x = ((int)mpu6050_read_reg(MPU6050_GYRO_XOUT_H) << 8) | mpu6050_read_reg(MPU6050_GYRO_XOUT_L);
    data->y = ((int)mpu6050_read_reg(MPU6050_GYRO_YOUT_H) << 8) | mpu6050_read_reg(MPU6050_GYRO_YOUT_L);
    data->z = ((int)mpu6050_read_reg(MPU6050_GYRO_ZOUT_H) << 8) | mpu6050_read_reg(MPU6050_GYRO_ZOUT_L);
}

void mpu6050_read_accel(accel_data_t *data) {
    data->x = ((int)mpu6050_read_reg(MPU6050_ACCEL_XOUT_H) << 8) | mpu6050_read_reg(MPU6050_ACCEL_XOUT_L);
    data->y = ((int)mpu6050_read_reg(MPU6050_ACCEL_YOUT_H) << 8) | mpu6050_read_reg(MPU6050_ACCEL_YOUT_L);
    data->z = ((int)mpu6050_read_reg(MPU6050_ACCEL_ZOUT_H) << 8) | mpu6050_read_reg(MPU6050_ACCEL_ZOUT_L);
}
