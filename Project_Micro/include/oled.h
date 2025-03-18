/**
 * SBC-OLED01 display interface (SSD1306 controller)
 */
#ifndef OLED_H
#define OLED_H

#include <stdint.h>

/* OLED Display constants */
#define OLED_ADDR             0x3C  // or 0x3D based on the module
#define OLED_ADDR_W           ((OLED_ADDR << 1) | 0)
#define OLED_ADDR_R           ((OLED_ADDR << 1) | 1)
#define OLED_CMD_MODE         0x00  // Command mode
#define OLED_DATA_MODE        0x40  // Data mode

#define OLED_WIDTH            128
#define OLED_HEIGHT           64
#define OLED_PAGES            8     // OLED_HEIGHT / 8

/* OLED commands */
#define OLED_DISPLAY_OFF      0xAE
#define OLED_DISPLAY_ON       0xAF
#define OLED_DISPLAY_NORMAL   0xA6
#define OLED_DISPLAY_INVERT   0xA7
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

/* Function prototypes */
void oled_init(void);
void oled_command(uint8_t cmd);
void oled_data(uint8_t data);
void oled_set_position(uint8_t x, uint8_t page);
void oled_clear(void);
void oled_display_on(void);
void oled_display_off(void);
void oled_display_char(uint8_t x, uint8_t page, char ch);
void oled_display_string(uint8_t x, uint8_t page, const char *str);
void oled_display_int(uint8_t x, uint8_t page, int16_t value);
void oled_draw_pixel(uint8_t x, uint8_t y, uint8_t color);
void oled_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void oled_draw_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void oled_draw_graph(int16_t data[], uint8_t size, uint8_t page);

#endif /* OLED_H */
