#include "pcf8574.h"
#include <stdint.h>
#include "i2c.h"

void pcf8574_init(void) {
    // All pins as outputs (low)
    pcf8574_write(0xFF); // Set all pins high initially (common cathode display is off)
}

void pcf8574_write(uint8_t data) {
    i2c_start(PCF8574_ADDR_W);
    i2c_write(data);
    i2c_stop();
}

uint8_t pcf8574_read(void) {
    uint8_t data;
    
    i2c_start(PCF8574_ADDR_R);
    data = i2c_read_nack();
    i2c_stop();
    
    return data;
}
