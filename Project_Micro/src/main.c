/**
 * Main program for the Gyroscope with OLED display project
 * 
 * Hardware: 
 * - Arduino/AVR board
 * - MPU6050 Gyroscope/Accelerometer
 * - SBC-OLED01 OLED Display
 * 
 * Author: For Doufless1, 2025-03-18
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "../include/i2c.h"
#include "../include/mpu6050.h"
#include "../include/oled.h"
#include "../include/util.h"

/* Data collection settings */
#define DATA_POINTS           50    // Number of data points for the graph
#define SAMPLE_INTERVAL_MS    20    // Sample interval in milliseconds

/* Function prototypes */
void display_splash_screen(void);
void display_gyro_data(gyro_data_t *data);
void collect_graph_data(int16_t data[], uint8_t size);

/**
 * Main program entry point
 */
int main(void) {
    // Initialize I2C communication interface
    i2c_init();
    
    // Initialize devices
    oled_init();
    
    // Initialize MPU6050 and check if it's found
    if (mpu6050_init()) {
        // Display error on OLED if MPU6050 not found
        oled_display_string(0, 0, "ERROR:");
        oled_display_string(0, 1, "MPU6050 not found");
        oled_display_string(0, 3, "Check connections");
        while (1); // Halt
    }
    
    // Show splash screen
    display_splash_screen();
    _delay_ms(2000);
    
    // Buffer for graph data
    int16_t graph_data[DATA_POINTS];
    
    // Main program loop
    while (1) {
        // Clear display
        oled_clear();
        
        // Display title
        oled_display_string(0, 0, "MPU6050 Gyroscope");
        oled_draw_line(0, 10, 127, 10);
        
        // Read current gyro data and display
        gyro_data_t current_data;
        mpu6050_read_gyro(&current_data);
        display_gyro_data(&current_data);
        
        // Collect data for graph
        collect_graph_data(graph_data, DATA_POINTS);
        
        // Draw graph
        oled_draw_graph(graph_data, DATA_POINTS, 4);
        
        // Wait a bit before refreshing
        _delay_ms(100);
    }
    
    return 0; // Never reached
}

/**
 * Display splash screen on OLED
 */
void display_splash_screen(void) {
    oled_clear();
    oled_display_string(10, 1, "MPU6050");
    oled_display_string(20, 2, "with");
    oled_display_string(5, 3, "SBC-OLED01");
    oled_display_string(0, 5, "Doufless1");
    oled_display_string(0, 6, "2025-03-18");
}

/**
 * Display gyroscope data on OLED
 */
void display_gyro_data(gyro_data_t *data) {
    // Convert to degrees/second (131 LSB per deg/s at ±250 deg/s range)
    float x_dps = data->x / 131.0;
    float y_dps = data->y / 131.0;
    float z_dps = data->z / 131.0;
    
    // Display X axis
    oled_display_string(0, 1, "X:");
    oled_display_int(20, 1, (int16_t)x_dps);
    oled_display_string(80, 1, "deg/s");
    
    // Display Y axis
    oled_display_string(0, 2, "Y:");
    oled_display_int(20, 2, (int16_t)y_dps);
    oled_display_string(80, 2, "deg/s");
    
    // Display Z axis
    oled_display_string(0, 3, "Z:");
    oled_display_int(20, 3, (int16_t)z_dps);
    oled_display_string(80, 3, "deg/s");
}

/**
 * Collect gyroscope data for graphing
 * data: Buffer to store collected data
 * size: Number of data points to collect
 */
void collect_graph_data(int16_t data[], uint8_t size) {
    gyro_data_t gyro_data;
    
    for (uint8_t i = 0; i < size; i++) {
        mpu6050_read_gyro(&gyro_data);
        data[i] = gyro_data.x / 131; // Use X-axis rotation in deg/s
        _delay_ms(SAMPLE_INTERVAL_MS);
    }
}
