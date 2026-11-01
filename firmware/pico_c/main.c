#include "config.h"

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "protocol.h"
#include "ping/console.h"
#include "ping/display.h"
#include "ping/radio.h"
#include "ping/buzzer.h"

#define LOOP_DELAY_MS 5
#define DEDUPE_LIMIT 64
#define BENCHMARK 1

typedef struct { uint8_t src; uint16_t seq; } seen_key_t;

int main(void) {
    stdio_init_all();

    console_t console;
    display_t display;
    radio_t radio;
    buzzer_t buzzer;

    console_init(&console);
    display_init(&display);
    radio_init(&radio);
    buzzer_init(&buzzer);

    char status[32];
    snprintf(status, sizeof(status), "node %d ready", NODE_ID);
    display_show_status(&display, "P.I.N.G", status);
    char ready[64];
    snprintf(ready, sizeof(ready), "P.I.N.G node %d ready. Type text to broadcast, @<id> text to direct.", NODE_ID);
    console_println(ready);

    seen_key_t seen[DEDUPE_LIMIT + 1];
    int seen_count = 0;

    uint8_t rxbuf[PAYLOAD_SIZE];

#if BENCHMARK
    uint32_t loop_count = 0;
    absolute_time_t bench_start = get_absolute_time();
#endif

    while (true) {
        char line[CONSOLE_MAX_LINE + 1];
        if (console_poll_line(&console, line, sizeof(line))) {
            uint8_t dst;
            char text[CONSOLE_MAX_LINE + 1];
            if (!ping_parse_command(line, &dst, text, sizeof(text))) {
                console_println("[err] use: text  or  @<id> text");
            } else {
                uint16_t seq = radio_next_seq(&radio);
                uint8_t packet[PAYLOAD_SIZE];
                if (ping_text_message((uint8_t)NODE_ID, dst, seq, text, 0, packet) == 0) {
                    int ok = radio_send(&radio, packet);
                    if (ok) buzzer_beep(&buzzer, 1, 80, 80);
                    char target[16];
                    if (dst == PING_BROADCAST) strcpy(target, "all");
                    else snprintf(target, sizeof(target), "%d", dst);
                    char out[256];
                    snprintf(out, sizeof(out), "[tx] -> %s: %s%s", target, text, ok ? "" : " (no ack)");
                    console_println(out);
                    display_add_message(&display, "tx>", text);
                }
            }
        }

        if (radio_poll(&radio, rxbuf)) {
            ping_message_t msg;
            if (ping_decode(rxbuf, PAYLOAD_SIZE, &msg) == 0) {
                if (msg.type == PING_TYPE_TEXT && (msg.dst == NODE_ID || msg.dst == PING_BROADCAST)) {
                    int found = 0;
                    for (int i = 0; i < seen_count; i++) {
                        if (seen[i].src == msg.src && seen[i].seq == msg.seq) { found = 1; break; }
                    }
                    if (!found) {
                        if (seen_count < DEDUPE_LIMIT) {
                            seen[seen_count].src = msg.src;
                            seen[seen_count].seq = msg.seq;
                            seen_count++;
                        } else {
                            seen_count = 0;
                            seen[0].src = msg.src;
                            seen[0].seq = msg.seq;
                            seen_count = 1;
                        }
                        char label[16];
                        snprintf(label, sizeof(label), "@%d", msg.src);
                        display_add_message(&display, label, msg.text);
                        char out[256];
                        snprintf(out, sizeof(out), "[rx] %s %s", label, msg.text);
                        console_println(out);
                        buzzer_beep(&buzzer, 2, 80, 80);
                    }
                }
            } else {
                console_println("[rx] bad packet");
            }
        }

#if BENCHMARK
        loop_count++;
        if (loop_count % 5000 == 0) {
            uint64_t us = to_us_since_boot(get_absolute_time()) - to_us_since_boot(bench_start);
            char bench[128];
            snprintf(bench, sizeof(bench), "[bench] loops=%lu elapsed_us=%llu loop_us=%.1f",
                     (unsigned long)loop_count, (unsigned long long)us,
                     (double)us / (double)loop_count);
            console_println(bench);
        }
#endif

        sleep_ms(LOOP_DELAY_MS);
    }
    return 0;
}
