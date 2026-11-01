#ifndef PING_DISPLAY_H
#define PING_DISPLAY_H

#include "ssd1306.h"

#define DISPLAY_MAX_LINES 32
#define DISPLAY_LINE_LEN 64

typedef struct {
    ssd1306_t oled;
    int line_count;
    char lines[DISPLAY_MAX_LINES][DISPLAY_LINE_LEN];
} display_t;

void display_init(display_t *d);
void display_show_status(display_t *d, const char *line1, const char *line2);
void display_add_message(display_t *d, const char *label, const char *text);

#endif /* PING_DISPLAY_H */
