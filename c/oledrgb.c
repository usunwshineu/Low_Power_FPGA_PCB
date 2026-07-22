#include <stdint.h>
#include "up5k_riscv.h"
#include "spi.h"
#include "clkcnt.h"
#include "oledrgb.h"
#include "font_8x8.h"

#define OLED_W 96
#define OLED_H 64

static void oled_cmd(uint8_t cmd)
{
    gp_out &= ~OLED_DC;
    spi_tx_byte(SPI1, cmd);
}

static void oled_cmd2(uint8_t cmd, uint8_t a)
{
    gp_out &= ~OLED_DC;
    spi_cs_low(SPI1);
    spi_transmit(SPI1, &cmd, 1);
    spi_transmit(SPI1, &a, 1);
    spi_cs_high(SPI1);
}

static void oled_cmd3(uint8_t cmd, uint8_t a, uint8_t b)
{
    gp_out &= ~OLED_DC;
    spi_cs_low(SPI1);
    spi_transmit(SPI1, &cmd, 1);
    spi_transmit(SPI1, &a, 1);
    spi_transmit(SPI1, &b, 1);
    spi_cs_high(SPI1);
}

static void oled_cmd5(uint8_t cmd, uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    uint8_t buf[5] = {cmd, a, b, c, d};

    gp_out &= ~OLED_DC;
    spi_cs_low(SPI1);
    spi_transmit(SPI1, buf, 5);
    spi_cs_high(SPI1);
}

void oled_drawstr(uint8_t x, uint8_t y, const char *str, uint16_t fg, uint16_t bg)
{
    while(*str && (x + 8) <= OLED_W) {
        oled_drawchar(x, y, *str++, fg, bg);
        x += 8;
    }
}

void oled_drawchar(uint8_t x, uint8_t y, char c, uint16_t fg, uint16_t bg)
{
    const uint8_t *glyph = &fontdata[(uint8_t)c * 8];
    uint8_t fg_hi = fg >> 8, fg_lo = fg & 0xff; //forground color; uses 2 x 8bits instead of 16 bits
    uint8_t bg_hi = bg >> 8, bg_lo = bg & 0xff; //background color
    uint8_t row, col;

    oled_cmd3(0x15, x, x + 7); // set column address
    oled_cmd3(0x75, y, y + 7); // set row address

    gp_out |= OLED_DC;
    spi_cs_low(SPI1);
    for(row = 0; row < 8; row++) {
        uint8_t bits = glyph[row];
        for(col = 0; col < 8; col++) {
            if(bits & (0x80 >> col)) {
                spi_transmit(SPI1, &fg_hi, 1);
                spi_transmit(SPI1, &fg_lo, 1);
            } else {
                spi_transmit(SPI1, &bg_hi, 1);
                spi_transmit(SPI1, &bg_lo, 1);
            }
        }
    }
    spi_cs_high(SPI1);
}

void oled_init(void)
{
    spi_init(SPI1);

    gp_out &= ~(OLED_DC | OLED_RES | OLED_VCCEN | OLED_PMODEN);

    gp_out |= OLED_RES;
    gp_out |= OLED_PMODEN;
    clkcnt_delayms(20);

    gp_out &= ~OLED_RES;
    clkcnt_delayms(1);

    gp_out |= OLED_RES;
    clkcnt_delayms(1);

    oled_cmd2(0xFD, 0x12);
    oled_cmd(0xAE);
    oled_cmd2(0xA0, 0x72);
    oled_cmd2(0xA1, 0x00);
    oled_cmd2(0xA2, 0x00);
    oled_cmd(0xA4);
    oled_cmd2(0xA8, 0x3F);
    oled_cmd2(0xAD, 0x8E);
    oled_cmd2(0xB0, 0x0B);
    oled_cmd2(0xB1, 0x31);
    oled_cmd2(0xB3, 0xF0);
    oled_cmd2(0x8A, 0x64);
    oled_cmd2(0x8B, 0x78);
    oled_cmd2(0x8C, 0x64);
    oled_cmd2(0xBB, 0x3A);
    oled_cmd2(0xBE, 0x3E);
    oled_cmd2(0x87, 0x06);
    oled_cmd2(0x81, 0x91);
    oled_cmd2(0x82, 0x50);
    oled_cmd2(0x83, 0x7D);
    oled_cmd(0x2E);
    oled_cmd5(0x25, 0x00, 0x00, 0x5F, 0x3F);

    gp_out |= OLED_VCCEN;
    clkcnt_delayms(25);

    oled_cmd(0xAF);
    clkcnt_delayms(100);
}

void oled_fill_rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color)
{
    uint16_t pixels;
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xff;

    oled_cmd3(0x15, x0, x1);   // column address
    oled_cmd3(0x75, y0, y1);   // row address

    gp_out |= OLED_DC;
    spi_cs_low(SPI1);

    pixels = (x1 - x0 + 1) * (y1 - y0 + 1);

    while (pixels--) {
        spi_transmit(SPI1, &hi, 1);
        spi_transmit(SPI1, &lo, 1);
    }

    spi_cs_high(SPI1);
}

void oled_clear(uint16_t color)
{
    oled_fill_rect(0, 0, OLED_W - 1, OLED_H - 1, color);
}