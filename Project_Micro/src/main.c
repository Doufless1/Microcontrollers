/**
 * Main program for the Gyroscope with OLED display project
 * 
 * Hardware: 
 * - Arduino/AVR board
 * - MPU6050 Gyroscope/Accelerometer
 * - SBC-OLED01 OLED Display
 * 
 * Author: For Doufless1, 2025-03-20
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include "../include/i2c.h"
#include "../include/mpu6050.h"
#include "../include/oled.h"
#include "../include/util.h"
#include "../include/uart.h"

/* Data collection settings */
#define DATA_POINTS           50    // Number of data points for the graph
#define SAMPLE_INTERVAL_MS    20    // Sample interval in milliseconds

/* UART settings */
#define UART_BAUD            9600   // Baud rate for UART communication

/* Display flags */
uint8_t show_graph = 1;             // Whether to show the graph
uint8_t show_accelerometer = 1;     // Whether to show accelerometer data
uint8_t invert_display = 0;         // Whether to invert display

/* Function prototypes */
void display_splash_screen(void);
void display_sensor_data(gyro_data_t *gyro, accel_data_t *accel);
void collect_graph_data(int data[], int size);
void display_command_notification(const char *cmd);
void process_uart_command(void);

/**
 * Main program entry point
 */
int main(void) {
    // Initialize UART
    uart_init(UART_BAUD);
    uart_puts("\r\nMPU6050 with OLED and UART Control\r\n");
    uart_puts("Available commands:\r\n");
    uart_puts("  graph on|off - Toggle graph display\r\n");
    uart_puts("  accel on|off - Toggle accelerometer display\r\n");
    uart_puts("  invert on|off - Toggle display inversion\r\n");
    uart_puts("  help - Show this help\r\n");
    
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
    int graph_data[DATA_POINTS];
    
    // Main program loop
    while (1) {
        // Check for UART commands
        if (uart_process_command()) {
            process_uart_command();
        }
        
        // Clear display
        oled_clear();
        
        // Read current sensor data
        gyro_data_t gyro_data;
        accel_data_t accel_data;
        mpu6050_read_gyro(&gyro_data);
        
        if (show_accelerometer) {
            mpu6050_read_accel(&accel_data);
        }
        
        // Display sensor data
        display_sensor_data(&gyro_data, show_accelerometer ? &accel_data : NULL);
        
        // Collect and display graph data if enabled
        if (show_graph) {
            collect_graph_data(graph_data, DATA_POINTS);
            oled_draw_graph(graph_data, DATA_POINTS, 6);
        }
        
        // Apply inversion if set
        if (invert_display) {
            oled_invert_display(1);
        } else {
            oled_invert_display(0);
        }
        
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
    oled_display_string(10, 1, "Konstantin");
    oled_display_string(20, 2, "Apostolov");
    oled_display_string(5, 3, "SBC-OLED01");
    oled_display_string(0, 5, "Microcontrollers");
    oled_display_string(0, 6, "UART Control");
    oled_display_string(0, 7, "2025-03-20");
}

/**
 * Display both gyroscope and accelerometer data on OLED
 */
void display_sensor_data(gyro_data_t *gyro, accel_data_t *accel) {
    // Display title
    oled_display_string(0, 0, "MPU6050 Sensor Data");
    oled_display_string(0, 7, "UART: 9600 baud");
    oled_draw_line(0, 10, 127, 10);
    
    // Convert to degrees/second (131 LSB per deg/s at ±250 deg/s range)
    float x_dps = gyro->x / 131.0;
    float y_dps = gyro->y / 131.0;
    float z_dps = gyro->z / 131.0;
    
    // Display gyroscope data
    oled_display_string(0, 1, "GYRO:");
    
    oled_display_string(0, 2, "X:");
    oled_display_int(15, 2, (int)x_dps);
    
    oled_display_string(0, 3, "Y:");
    oled_display_int(15, 3, (int)y_dps);
    
    oled_display_string(0, 4, "Z:");
    oled_display_int(15, 4, (int)z_dps);
    
    // Display units
    oled_display_string(0, 5, "(deg/s)");
    
    // Display accelerometer data if enabled
    if (accel != NULL) {
        // Convert to g force (16384 LSB per g at ±2g range)
        // Multiplied by 100 to show as centi-g (hundredths of g)
        int x_cg = (int)(accel->x / 16384.0 * 100);
        int y_cg = (int)(accel->y / 16384.0 * 100);
        int z_cg = (int)(accel->z / 16384.0 * 100);
        
        // Display data
        oled_display_string(64, 1, "ACCEL:");
        
        oled_display_string(64, 2, "X:");
        oled_display_int(79, 2, x_cg);
        
        oled_display_string(64, 3, "Y:");
        oled_display_int(79, 3, y_cg);
        
        oled_display_string(64, 4, "Z:");
        oled_display_int(79, 4, z_cg);
        
        // Display units
        oled_display_string(64, 5, "(0.01g)");
    }
}

/**
 * Collect gyroscope data for graphing
 * data: Buffer to store collected data
 * size: Number of data points to collect
 */
void collect_graph_data(int data[], int size) {
    gyro_data_t gyro_data;
    
    for (int i = 0; i < size; i++) {
        mpu6050_read_gyro(&gyro_data);
        data[i] = gyro_data.x / 131; // Use X-axis rotation in deg/s
        _delay_ms(SAMPLE_INTERVAL_MS);
    }
}

/**
 * Display command notification on OLED
 */
void display_command_notification(const char *cmd) {
    // Save current display content
    oled_invert_display(1);
    _delay_ms(100);
    oled_invert_display(0);
    
    // Show command notification
    oled_clear();
    oled_display_string(0, 2, "UART Command:");
    oled_display_string(0, 4, cmd);
    _delay_ms(1000);
}

/**
 * Process received UART command
 */
void process_uart_command(void) {
    // Get command and parameter from UART
    char *cmd = current_command;
    int param = command_param;
    
    // Flash screen to show command received
    display_command_notification(cmd);
    
    // Process command
    if (strcmp(cmd, "graph") == 0) {
        if (param == 1 || strcmp(cmd + 6, "on") == 0) {
            show_graph = 1;
            uart_puts("Graph display enabled\r\n");
        } else {
            show_graph = 0;
            uart_puts("Graph display disabled\r\n");
        }
    }
    else if (strcmp(cmd, "accel") == 0) {
        if (param == 1 || strcmp(cmd + 6, "on") == 0) {
            show_accelerometer = 1;
            uart_puts("Accelerometer display enabled\r\n");
        } else {
            show_accelerometer = 0;
            uart_puts("Accelerometer display disabled\r\n");
        }
    }
    else if (strcmp(cmd, "invert") == 0) {
        if (param == 1 || strcmp(cmd + 7, "on") == 0) {
            invert_display = 1;
            uart_puts("Display inversion enabled\r\n");
        } else {
            invert_display = 0;
            uart_puts("Display inversion disabled\r\n");
        }
    }
    else if (strcmp(cmd, "help") == 0) {
        uart_puts("Available commands:\r\n");
        uart_puts("  graph on|off - Toggle graph display\r\n");
        uart_puts("  accel on|off - Toggle accelerometer display\r\n");
        uart_puts("  invert on|off - Toggle display inversion\r\n");
        uart_puts("  help - Show this help\r\n");
    }
    else {
        uart_puts("Unknown command. Type 'help' for available commands\r\n");
    }
}
