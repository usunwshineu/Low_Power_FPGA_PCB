#ifndef __OLEDRGB_H__
#define __OLEDRGB_H__

#include <stdint.h>

#define OLED_DC      (1u << 20)
#define OLED_RES     (1u << 21)
#define OLED_VCCEN   (1u << 22)
#define OLED_PMODEN  (1u << 23)
#define BLACK   0x0000
#define WHITE   0xFFFF
#define CYAN    0x07FF
#define YELLOW  0xFFE0

void oled_init(void);
void oled_clear(uint16_t color);
void oled_fill_rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color);
void oled_drawchar(uint8_t x, uint8_t y, char c, uint16_t fg, uint16_t bg);
void oled_drawstr(uint8_t x, uint8_t y, const char *str, uint16_t fg, uint16_t bg);

#endif