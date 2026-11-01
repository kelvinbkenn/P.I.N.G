#include "buzzer.h"

#include "config.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"

void buzzer_init(buzzer_t *b) {
    b->pin = BUZZER_PIN;
    gpio_init(b->pin);
    gpio_set_dir(b->pin, GPIO_OUT);
    gpio_put(b->pin, 0);
}

void buzzer_beep(buzzer_t *b, int count, int on_ms, int off_ms) {
    for (int i = 0; i < count; i++) {
        gpio_put(b->pin, 1);
        sleep_ms(on_ms);
        gpio_put(b->pin, 0);
        if (i + 1 < count) {
            sleep_ms(off_ms);
        }
    }
}
