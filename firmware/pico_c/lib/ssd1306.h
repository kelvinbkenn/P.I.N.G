#ifndef PING_SSD1306_H
#define PING_SSD1306_H

#include <stdint.h>

#include "hardware/i2c.h"

typedef struct {
    int width;
    int height;
    int pages;
    int external_vcc;
    void *i2c; /* i2c_inst_t* */
    uint8_t addr;
    uint8_t buffer[128 * 64 / 8];
} ssd1306_t;

void ssd1306_init(ssd1306_t *d, int width, int height, void *i2c, uint8_t addr,
                  int external_vcc);
void ssd1306_fill(ssd1306_t *d, uint8_t color);
void ssd1306_show(ssd1306_t *d);
void ssd1306_text(ssd1306_t *d, const char *str, int x, int y);
void ssd1306_hline(ssd1306_t *d, int x, int y, int w, uint8_t color);

#endif /* PING_SSD1306_H */
