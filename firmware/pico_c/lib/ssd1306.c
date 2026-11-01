#include "ssd1306.h"
#include "font8x8.h"

#include <string.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

static void ssd1306_command(ssd1306_t *d, uint8_t cmd) {
    i2c_inst_t *i2c = (i2c_inst_t *)d->i2c;
    uint8_t buf[2] = { 0x00, cmd };
    i2c_write_blocking(i2c, d->addr, buf, 2, false);
}

void ssd1306_init(ssd1306_t *d, int width, int height, void *i2c, uint8_t addr,
                  int external_vcc) {
    d->width = width;
    d->height = height;
    d->pages = height / 8;
    d->external_vcc = external_vcc;
    d->i2c = i2c;
    d->addr = addr;
    memset(d->buffer, 0, (size_t)width * (size_t)(height / 8));

    ssd1306_command(d, 0xAE); /* display off */
    ssd1306_command(d, 0xD5);
    ssd1306_command(d, 0x80);
    ssd1306_command(d, 0xA8);
    ssd1306_command(d, (uint8_t)(height - 1));
    ssd1306_command(d, 0xD3);
    ssd1306_command(d, 0x00);
    ssd1306_command(d, 0x40);
    ssd1306_command(d, 0x8D);
    ssd1306_command(d, 0x14);
    ssd1306_command(d, 0x20);
    ssd1306_command(d, 0x00);
    ssd1306_command(d, 0xA1);
    ssd1306_command(d, 0xC8);
    ssd1306_command(d, 0xDA);
    ssd1306_command(d, (uint8_t)(width == 128 ? 0x12 : 0x02));
    ssd1306_command(d, 0x81);
    ssd1306_command(d, 0xCF);
    ssd1306_command(d, 0xD9);
    ssd1306_command(d, 0xF1);
    ssd1306_command(d, 0xDB);
    ssd1306_command(d, 0x40);
    ssd1306_command(d, 0xA4);
    ssd1306_command(d, 0xA6);
    ssd1306_command(d, 0xAF); /* display on */
}

void ssd1306_fill(ssd1306_t *d, uint8_t color) {
    memset(d->buffer, color ? 0xFF : 0x00, (size_t)d->width * (size_t)(d->height / 8));
}

void ssd1306_show(ssd1306_t *d) {
    i2c_inst_t *i2c = (i2c_inst_t *)d->i2c;
    for (int page = 0; page < d->pages; page++) {
        ssd1306_command(d, (uint8_t)(0xB0 + page));
        ssd1306_command(d, 0x00);
        ssd1306_command(d, 0x10);
        uint8_t buf[1 + 128];
        buf[0] = 0x40;
        memcpy(buf + 1, &d->buffer[page * d->width], d->width);
        i2c_write_blocking(i2c, d->addr, buf, (size_t)(1 + d->width), false);
    }
}

void ssd1306_text(ssd1306_t *d, const char *str, int x, int y) {
    if (str == NULL) return;
    int cursor_x = x;
    for (size_t i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];
        if (c < 32 || c > 127) c = '?';
        const uint8_t *glyph = ping_font8x8[c - 32];
        for (int row = 0; row < 8; row++) {
            uint8_t bits = glyph[row];
            for (int col = 0; col < 8; col++) {
                if (bits & (uint8_t)(0x80 >> col)) {
                    int px = cursor_x + col;
                    int py = y + row;
                    if (px < 0 || px >= d->width || py < 0 || py >= d->height) continue;
                    d->buffer[(py / 8) * d->width + px] |= (uint8_t)(1 << (py % 8));
                }
            }
        }
        cursor_x += 8;
        if (cursor_x + 8 > d->width) break;
    }
}

void ssd1306_hline(ssd1306_t *d, int x, int y, int w, uint8_t color) {
    if (y < 0 || y >= d->height) return;
    for (int i = 0; i < w; i++) {
        int px = x + i;
        if (px < 0 || px >= d->width) continue;
        if (color) {
            d->buffer[(y / 8) * d->width + px] |= (uint8_t)(1 << (y % 8));
        } else {
            d->buffer[(y / 8) * d->width + px] &= (uint8_t)~(1 << (y % 8));
        }
    }
}
