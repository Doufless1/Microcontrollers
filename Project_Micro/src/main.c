#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include "../include/i2c.h"
#include "../include/mpu6050.h"
#include "../include/oled.h"
#include "../include/util.h"
#include "../include/uart.h"


#define UART_BAUD            9600   


uint8_t show_accelerometer = 1;     
uint8_t invert_display = 0;         
uint8_t refresh_rate = 5;           


void display_splash_screen(void);
void display_sensor_data(gyro_data_t *gyro, accel_data_t *accel);
void display_command_notification(const char *cmd);
void process_uart_command(void);

int main(void) {
    uart_init(UART_BAUD);
    uart_puts("\r\nMPU6050 with OLED and UART Control\r\n");
    uart_puts("Available commands:\r\n");
    uart_puts("  accel on|off - Toggle accelerometer display\r\n");
    uart_puts("  invert on|off - Toggle display inversion\r\n");
    uart_puts("  refresh <rate> - Set refresh rate (1-100, in tenths of seconds)\r\n");
    uart_puts("  help - Show this help\r\n");
    
    
    i2c_init();
    
    
    oled_init();
    
    
    if (mpu6050_init()) {
        
        oled_display_string(0, 0, "ERROR:");
        oled_display_string(0, 1, "MPU6050 not found");
        oled_display_string(0, 3, "Check connections");
        while (1); 
    }
    
    
    display_splash_screen();
    _delay_ms(2000);
    
    
    uint32_t last_update_time = 0;
    uint32_t current_time = 0;
    
    
    while (1) {
        
        if (uart_process_command()) {
            process_uart_command();
            last_update_time = 0;  // Force an update after a command
        }
        
        // Increment the time counter (each loop is approximately 10ms)
        current_time++;
        
        
        if ((current_time - last_update_time) >= refresh_rate) {
            
            oled_clear();
            
            
            gyro_data_t gyro_data;
            accel_data_t accel_data;
            mpu6050_read_gyro(&gyro_data);
            
            if (show_accelerometer) {
                mpu6050_read_accel(&accel_data);
            }
            
            
            display_sensor_data(&gyro_data, show_accelerometer ? &accel_data : NULL);
            
            
            if (invert_display) {
                oled_invert_display(1);
            } else {
                oled_invert_display(0);
            }
            
            
            last_update_time = current_time;
        }
        
        
        _delay_ms(10);
    }
    
    return 0; 
}

void display_splash_screen(void) {
    oled_clear();
    oled_display_string(10, 1, "Konstantin");
    oled_display_string(20, 2, "Apostolov");
    oled_display_string(5, 3, "SBC-OLED01");
    oled_display_string(0, 5, "Microcontrollers");
    oled_display_string(0, 6, "UART Control");
    oled_display_string(0, 7, "2025-03-26");
}

void display_sensor_data(gyro_data_t *gyro, accel_data_t *accel) {
    
    oled_display_string(0, 0, "MPU6050 Sensor Data");
    
    
    char info[21];
    sprintf(info, "Refresh: %d.%ds", refresh_rate / 10, refresh_rate % 10);
    oled_display_string(0, 7, info);
    oled_display_string(80, 7, "9600bd");
    
    oled_draw_line(0, 10, 127, 10);
    
    // Convert to degrees/second (131 LSB per deg/s at ±250 deg/s range)
    float x_dps = gyro->x / 131.0;
    float y_dps = gyro->y / 131.0;
    float z_dps = gyro->z / 131.0;
    
    
    oled_display_string(0, 1, "GYRO:");
    
    oled_display_string(0, 2, "X:");
    oled_display_int(15, 2, (int)x_dps);
    
    oled_display_string(0, 3, "Y:");
    oled_display_int(15, 3, (int)y_dps);
    
    oled_display_string(0, 4, "Z:");
    oled_display_int(15, 4, (int)z_dps);
    
    
    oled_display_string(0, 5, "(deg/s)");
    
    
    if (accel != NULL) {
        // convert to g force (16384 LSB per g at ±2g range)
        // multiplied by 100 to show as centi-g (hundredths of g)
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
        
        
        oled_display_string(64, 5, "(0.01g)");
    }
}

void display_command_notification(const char *cmd) {
    
    oled_invert_display(1);
    _delay_ms(100);
    oled_invert_display(0);
    
    
    oled_clear();
    oled_display_string(0, 2, "UART Command:");
    oled_display_string(0, 4, cmd);
    _delay_ms(2000); 
}


void process_uart_command(void) {
    
    char *cmd = current_command;
    int param = command_param;
    
    
    display_command_notification(cmd);
    
   
    if (strcmp(cmd, "accel") == 0) {
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
    else if (strcmp(cmd, "refresh") == 0) {
        if (param >= 1 && param <= 100) {
            refresh_rate = param;
            uart_puts("Refresh rate set to ");
            uart_puti(param / 10);
            uart_putc('.');
            uart_puti(param % 10);
            uart_puts(" seconds\r\n");
        } else {
            uart_puts("Invalid refresh rate. Use a value between 1-100 (0.1-10.0 seconds)\r\n");
        }
    }
    else if (strcmp(cmd, "help") == 0) {
        uart_puts("Available commands:\r\n");
        uart_puts("  accel on|off - Toggle accelerometer display\r\n");
        uart_puts("  invert on|off - Toggle display inversion\r\n");
        uart_puts("  refresh <rate> - Set refresh rate (1-50, in tenths of seconds)\r\n");
        uart_puts("  help - Show this help\r\n");
    }
    else {
        uart_puts("Unknown command. Type 'help' for available commands\r\n");
    }
}
