/**
 * SBC-OLED01 display implementation (SSD1306 controller)
 */
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "../include/oled.h"
#include "../include/i2c.h"
#include "../include/font.h"
#include "../include/util.h"

void oled_init(void) {
    
    _delay_ms(100);
    
    // flow chart page 64
    oled_command(OLED_DISPLAY_OFF);                   
    oled_command(OLED_SET_DISP_CLK_DIV);             // set clock divider
    oled_command(0x80);                              // default value
    oled_command(OLED_SET_MUX_RATIO);                // set multiplex ratio
    oled_command(0x3F);                              // 64 lines
    oled_command(OLED_SET_DISP_OFFSET);              // set display offset
    oled_command(0x00);                              // no offset
    oled_command(OLED_SET_DISP_START | 0x0);         // set display start line to 0
    oled_command(OLED_SET_CHARGE_PUMP);              // enable charge pump
    oled_command(0x14);
    oled_command(OLED_SET_MEM_ADDR);                 // set memory addressing mode
    oled_command(0x00);                              // horizontal addressing mode
    oled_command(OLED_SET_SEG_REMAP | 0x01);         // segment re-map
    oled_command(OLED_SET_COM_OUT_DIR);              // COM output direction
    oled_command(OLED_SET_COM_PIN);                  // COM pins hardware config
    oled_command(0x12);                              // alternative COM pin config
    oled_command(OLED_SET_CONTRAST);                 // set contrast
    oled_command(0xCF);                              // high contrast value
    oled_command(OLED_SET_PRECHARGE);                // set pre-charge period
    oled_command(0xF1);
    oled_command(OLED_SET_VCOM_DESEL);               // set VCOMH deselect level
    oled_command(0x40);
    oled_command(OLED_SET_ENTIRE_ON);                // output follows RAM content
    oled_command(OLED_SET_NORM_DISP);                
    oled_command(OLED_DISPLAY_ON);                   
    
    
    oled_clear();
}

// send command
void oled_command(int cmd) {
    i2c_start(OLED_ADDR_W);
    i2c_write(OLED_CMD_MODE);   // Control byte: Co=0, D/C=0 (command)
    i2c_write(cmd);
    i2c_stop();
}

// data to the oled
void oled_data(int data) {
    i2c_start(OLED_ADDR_W);
    i2c_write(OLED_DATA_MODE);   // Control byte: Co=0, D/C=1 (data)
    i2c_write(data);
    i2c_stop();
}

void oled_set_position(int x, int page) {
    if (page > 7) page = 7;
    if (x > 127) x = 127;
    
    oled_command(0xB0 + page);                   // Set page address
    oled_command(0x00 + (x & 0x0F));             // Set column address low nibble
    oled_command(0x10 + ((x >> 4) & 0x0F));      // Set column address high nibble
}

void oled_clear(void) {
    for (int page = 0; page < 8; page++) {
        oled_set_position(0, page);
        
        // send 128 bytes of zeros to clear the page
        i2c_start(OLED_ADDR_W);
        i2c_write(OLED_DATA_MODE);   // control byte for continuous data
        
        for (int x = 0; x < 128; x++) {
            i2c_write(0x00);
        }
        
        i2c_stop();
    }
}

void oled_display_on(void) {
    oled_command(OLED_DISPLAY_ON);
}


void oled_display_off(void) {
    oled_command(OLED_DISPLAY_OFF);
}


void oled_display_char(int x, int page, char ch) {
    if (ch < ' ' || ch > '~') {
        ch = ' ';  
    }
    
    // set cursor position
    oled_set_position(x, page);
    
    // calculate font index
    int idx = ch - ' ';
    
    // send font data
    i2c_start(OLED_ADDR_W);
    i2c_write(OLED_DATA_MODE);
    
    for (int i = 0; i < 5; i++) {
        i2c_write(pgm_read_byte(&font5x7[idx][i]));
    }
   // this is space 
    i2c_write(0x00);
    
    i2c_stop();
}


void oled_display_string(int x, int page, const char *str) {
    int pos_x = x;
    
    while (*str) {
        oled_display_char(pos_x, page, *str++);
        pos_x += 6;  // so each charecter given ias 5 pixels plus the space so 6
        
        if (pos_x > 120) {
            // we wrap it if it approches the end of the pixels for the given page
            pos_x = 0;
            page++;
            
            if (page > 7) {
                break;  // stop if we exceed the last page
            }
        }
    }
}


void oled_display_int(int x, int page, int value) {
    char buffer[7];  // max is 5 digits + sign + null
    int_to_str(value, buffer);
    oled_display_string(x, page, buffer);
}


void oled_draw_pixel(int x, int y, int color) {
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return;
    
    int page = y / 8;
    int bit = y % 8;
    int data;
    
    oled_set_position(x, page);
    
    // Read current data (Note: This is inefficient as it requires a read operation)
    // In a real implementation, you would maintain a frame buffer in RAM
    // For now, we'll just write the pixel without reading first
    data = 0;
    
    if (color) {
        data |= (1 << bit);
    } else {
        data &= ~(1 << bit);
    }
    
    // Write the modified data
    oled_set_position(x, page);
    oled_data(data);
}


void oled_draw_line(int x0, int y0, int x1, int y1) {
    int dx = abs_int16(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs_int16(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;
    
    while (1) {
        oled_draw_pixel(x0, y0, 1);
        
        if (x0 == x1 && y0 == y1) break;
        
        e2 = 2 * err;
        if (e2 >= dy) {
            if (x0 == x1) break;
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            if (y0 == y1) break;
            err += dx;
            y0 += sy;
        }
    }
}


void oled_draw_rect(int x, int y, int width, int height) {
    oled_draw_line(x, y, x + width - 1, y);
    oled_draw_line(x, y + height - 1, x + width - 1, y + height - 1);
    oled_draw_line(x, y, x, y + height - 1);
    oled_draw_line(x + width - 1, y, x + width - 1, y + height - 1);
}

/**
 * Draw a graph using data points
 * data: Array of data points
 * size: Number of data points
 * page: Starting page (0-6)
 */
void oled_draw_graph(int data[], int size, int page) {
    // Find min and max values for scaling
    int min_val = 32767;
    int max_val = -32768;
    
    for (int i = 0; i < size; i++) {
        if (data[i] < min_val) min_val = data[i];
        if (data[i] > max_val) max_val = data[i];
    }
    
    // Add some margin
    min_val -= 100;
    max_val += 100;
    
    // Avoid division by zero
    if (max_val == min_val) {
        max_val += 200;
        min_val -= 200;
    }
    
    // Calculate scale factor (using integer math to avoid float issues)
    int32_t scale_factor = 16 * 100 / (max_val - min_val); // scale * 100 for precision
    
    // Draw frame
    oled_draw_rect(0, page * 8, 127, 16);
    
    // Draw horizontal center line
    oled_draw_line(0, page * 8 + 8, 127, page * 8 + 8);
    
    // Draw graph
    for (int i = 0; i < size && i < 128; i++) {
        int y_pos = page * 8 + 8 - ((data[i] - min_val) * scale_factor) / 100;
        
        // Ensure y position is within bounds
        if (y_pos < page * 8) y_pos = page * 8;
        if (y_pos > page * 8 + 15) y_pos = page * 8 + 15;
        
        oled_draw_pixel(i, y_pos, 1);
    }
}

void oled_invert_display(uint8_t invert) {
    oled_command(invert ? 0xA7 : 0xA6); // A7=Invert, A6=Normal
}
