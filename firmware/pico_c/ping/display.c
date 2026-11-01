#include "display.h"

#include "config.h"

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

void display_init(display_t *d) {
    i2c_inst_t *i2c = (OLED_I2C_ID == 0) ? i2c0 : i2c1;
    i2c_init(i2c, 400000);
    gpio_set_function(OLED_SDA, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA);
    gpio_pull_up(OLED_SCL);

    ssd1306_init(&d->oled, OLED_WIDTH, OLED_HEIGHT, i2c, OLED_ADDR, 0);
    d->line_count = 0;
    for (int i = 0; i < DISPLAY_MAX_LINES; i++) {
        d->lines[i][0] = '\0';
    }
    display_show_status(d, "P.I.N.G", "booting");
}

void display_show_status(display_t *d, const char *line1, const char *line2) {
    ssd1306_fill(&d->oled, 0);
    if (line1) ssd1306_text(&d->oled, line1, 0, 0);
    if (line2) ssd1306_text(&d->oled, line2, 0, 12);
    ssd1306_show(&d->oled);
}

void display_add_message(display_t *d, const char *label, const char *text) {
    if (d->line_count < DISPLAY_MAX_LINES) {
        snprintf(d->lines[d->line_count], DISPLAY_LINE_LEN, "%s %s", label ? label : "", text ? text : "");
        d->line_count++;
    } else {
        for (int i = 1; i < DISPLAY_MAX_LINES; i++) {
            strncpy(d->lines[i - 1], d->lines[i], DISPLAY_LINE_LEN);
        }
        snprintf(d->lines[DISPLAY_MAX_LINES - 1], DISPLAY_LINE_LEN, "%s %s", label ? label : "", text ? text : "");
    }
    /* render last 5 lines */
    ssd1306_fill(&d->oled, 0);
    ssd1306_text(&d->oled, "P.I.N.G", 0, 0);
    ssd1306_hline(&d->oled, 0, 10, OLED_WIDTH, 1);
    int start = d->line_count > 5 ? d->line_count - 5 : 0;
    int y = 14;
    for (int i = start; i < d->line_count; i++) {
        char tmp[DISPLAY_LINE_LEN];
        strncpy(tmp, d->lines[i], DISPLAY_LINE_LEN - 1);
        tmp[DISPLAY_LINE_LEN - 1] = '\0';
        /* truncate to 16 chars */
        char shortbuf[17];
        snprintf(shortbuf, sizeof(shortbuf), "%.16s", tmp);
        ssd1306_text(&d->oled, shortbuf, 0, y);
        y += 10;
    }
    ssd1306_show(&d->oled);
}
