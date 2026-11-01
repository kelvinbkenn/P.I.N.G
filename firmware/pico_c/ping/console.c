#include "console.h"

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

void console_init(console_t *c) {
    c->max_line = CONSOLE_MAX_LINE;
    c->buffer[0] = '\0';
    c->len = 0;
}

int console_poll_line(console_t *c, char *line, size_t cap) {
    if (c == NULL || line == NULL) return 0;
    while (1) {
        int ch = getchar_timeout_us(0);
        if (ch == PICO_ERROR_TIMEOUT) break;
        if (ch == '\r' || ch == '\n') {
            if (c->len > 0) {
                size_t copy = c->len;
                if (copy + 1 > cap) copy = cap - 1;
                memcpy(line, c->buffer, copy);
                line[copy] = '\0';
                c->len = 0;
                c->buffer[0] = '\0';
                return 1;
            }
        } else if (ch == '\b' || ch == 0x7F) {
            if (c->len > 0) {
                c->len--;
                c->buffer[c->len] = '\0';
            }
        } else if (ch >= 32 && ch < 127) {
            if (c->len < CONSOLE_MAX_LINE && (size_t)(c->len + 1) < cap) {
                c->buffer[c->len++] = (char)ch;
                c->buffer[c->len] = '\0';
            }
        }
    }
    return 0;
}

void console_println(const char *text) {
    if (text == NULL) text = "";
    printf("%s\n", text);
}
