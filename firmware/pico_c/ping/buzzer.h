#ifndef PING_BUZZER_H
#define PING_BUZZER_H

#include <stdint.h>

typedef struct {
    int pin;
} buzzer_t;

void buzzer_init(buzzer_t *b);
void buzzer_beep(buzzer_t *b, int count, int on_ms, int off_ms);

#endif /* PING_BUZZER_H */
