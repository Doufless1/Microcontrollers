/**
 * SBC-OLED01 display interface (SSD1306 controller)
 */
#ifndef OLED_H
#define OLED_H

#include <stdint.h>

// OLED Display constants you can find on chaper 1.8.5
#define OLED_ADDR             0x3C  // page 20 of SSD1306
#define OLED_ADDR_W           ((OLED_ADDR << 1) | 0) // write opeariton
#define OLED_ADDR_R           ((OLED_ADDR << 1) | 1) // read operation
                                                     // here each adressed is shifted by 1 bit and the R/W bit is appended as LSB
#define OLED_CMD_MODE         0x00  // Command mode page 20 also
#define OLED_DATA_MODE        0x40  // Data mode
                                    //
                                    // When SA0 is connected to GND → Address is 0x3C (0111100b)
//                                   When SA0 is connected to VCC → Address is 0x3D (0111101b)

#define OLED_WIDTH            128
#define OLED_HEIGHT           64    
#define OLED_PAGES            8     // Height_of_display / 8 vertical pixels
                                    // each page(vertical line has 8 pixels)
                                    // so we get 8*8 = 64
                                    // Each column in a page stores 8 pixels as 1 byte (since 1 byte = 8 bits).
                                    // Since there are 128 columns per page, you have a 128x8 memory map for each page.
                                    //more details at 15-17 page


#define OLED_DISPLAY_OFF      0xAE // all the commands are on page 28
#define OLED_DISPLAY_ON       0xAF
#define OLED_DISPLAY_NORMAL   0xA6
#define OLED_DISPLAY_INVERT   0xA7
// from like page 32 are the other commands 
#define OLED_SET_CONTRAST     0x81
#define OLED_SET_NORM_DISP    0xA6
#define OLED_SET_ENTIRE_ON    0xA4
#define OLED_SET_MEM_ADDR     0x20
#define OLED_SET_COL_ADDR     0x21
#define OLED_SET_PAGE_ADDR    0x22
#define OLED_SET_DISP_START   0x40
#define OLED_SET_SEG_REMAP    0xA1
#define OLED_SET_MUX_RATIO    0xA8
#define OLED_SET_COM_OUT_DIR  0xC8
#define OLED_SET_DISP_OFFSET  0xD3
#define OLED_SET_COM_PIN      0xDA
#define OLED_SET_DISP_CLK_DIV 0xD5
#define OLED_SET_PRECHARGE    0xD9
#define OLED_SET_VCOM_DESEL   0xDB
#define OLED_SET_CHARGE_PUMP  0x8D

void oled_init(void);
void oled_command(int cmd);
void oled_data(int data);
void oled_set_position(int x, int page);
void oled_clear(void);
void oled_display_on(void);
void oled_display_off(void);
void oled_display_char(int x, int page, char ch);
void oled_display_string(int x, int page, const char *str);
void oled_display_int(int x, int page, int value);
void oled_draw_pixel(int x, int y, int color);
void oled_draw_line(int x0, int y0, int x1, int y1);
void oled_draw_rect(int x, int y, int width, int height);
void oled_draw_graph(int data[], int size, int page);
void oled_invert_display(uint8_t invert);

#endif /* OLED_H */
