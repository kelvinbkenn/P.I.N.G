#ifndef PING_CONSOLE_H
#define PING_CONSOLE_H

#include <stddef.h>

#define CONSOLE_MAX_LINE 200

typedef struct {
    int max_line;
    char buffer[CONSOLE_MAX_LINE + 1];
    int len;
} console_t;

void console_init(console_t *c);
/* returns 1 and fills line (null-terminated) if a full line is available */
int console_poll_line(console_t *c, char *line, size_t cap);
void console_println(const char *text);

#endif /* PING_CONSOLE_H */
