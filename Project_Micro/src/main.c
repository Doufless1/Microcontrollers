#include <avr/io.h>
#include <util/delay.h>
#include "../include/i2c.h"
#include "../include/mpu9265.h"
#include "../include/sa52_11ewa.h"
#include "../include/serial.h"

// Conversion factor for gyro data to degrees per second
// For ±250°/s range, sensitivity is 131 LSB/°/s
#define GYRO_SENSITIVITY 131.0

int main(void) {
    gyro_data_t gyro_data;
    uint8_t connection_status;
    
    // Initialize serial communication
    serial_init(9600);
    serial_tx_string("MPU-92/65 Gyroscope with SA52-11EWA Display Demo\r\n");
    
    // Initialize I2C
    i2c_init();
    
    // Test MPU-92/65 connection
    serial_tx_string("Testing connection to MPU-92/65... ");
    connection_status = mpu9265_test_connection();
    
    if (!connection_status) {
        serial_tx_string("FAILED!\r\n");
        serial_tx_string("Check connections and try again.\r\n");
        while (1); // Stop program execution
    } else {
        serial_tx_string("SUCCESS!\r\n");
    }
    
    // Initialize MPU-92/65
    serial_tx_string("Initializing MPU-92/65... ");
    if (mpu9265_init()) {
        serial_tx_string("SUCCESS!\r\n");
    } else {
        serial_tx_string("FAILED!\r\n");
        serial_tx_string("Check connections and try again.\r\n");
        while (1); // Stop program execution
    }
    
    // Initialize SA52-11EWA display
    serial_tx_string("Initializing SA52-11EWA display... ");
    sa52_11ewa_init();
    serial_tx_string("SUCCESS!\r\n");
    
    serial_tx_string("\r\nStarting gyroscope readings...\r\n\r\n");
    
    // Main loop
    while (1) {
        // Read gyroscope data
        mpu9265_read_gyro(&gyro_data);
        
        // Scale the gyroscope value for display (divide by 100 to show smaller values)
        int16_t display_value = gyro_data.x / 100;
        
        // Display X-axis gyro value on SA52-11EWA display
        sa52_11ewa_display_number(display_value);
        
        // Send data to serial monitor
        serial_tx_string("Gyroscope data (raw): ");
        serial_tx_string("X: ");
        serial_tx_int16(gyro_data.x);
        serial_tx_string(", Y: ");
        serial_tx_int16(gyro_data.y);
        serial_tx_string(", Z: ");
        serial_tx_int16(gyro_data.z);
        
        // Also send the calculated degrees per second
        serial_tx_string(" | Degrees/s: X: ");
        serial_tx_int16((int16_t)(gyro_data.x / GYRO_SENSITIVITY));
        serial_tx_string(", Y: ");
        serial_tx_int16((int16_t)(gyro_data.y / GYRO_SENSITIVITY));
        serial_tx_string(", Z: ");
        serial_tx_int16((int16_t)(gyro_data.z / GYRO_SENSITIVITY));
        serial_tx_string("\r\n");
        
        // Short delay between readings
        _delay_ms(500);
    }
    
    return 0;
}
