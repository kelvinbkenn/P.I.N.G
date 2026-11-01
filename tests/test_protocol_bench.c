#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "protocol.h"

static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1.0e9;
}

int main(void) {
    const int iterations = 1000000;
    uint8_t packet[PING_PAYLOAD_SIZE];
    ping_message_t msg;
    char text[64];

    /* encode benchmark */
    uint8_t dst = PING_BROADCAST;
    double start = now_seconds();
    for (int i = 0; i < iterations; i++) {
        ping_text_message((uint8_t)(i & 0xFF), dst, (uint16_t)i, "hello world", 0, packet);
    }
    double t_enc = now_seconds() - start;

    /* decode benchmark */
    ping_text_message(1, PING_BROADCAST, 42, "hello world", 0, packet);
    start = now_seconds();
    for (int i = 0; i < iterations; i++) {
        ping_decode(packet, PING_PAYLOAD_SIZE, &msg);
    }
    double t_dec = now_seconds() - start;

    /* parse benchmark */
    start = now_seconds();
    for (int i = 0; i < iterations; i++) {
        ping_parse_command("@3 hello world", &dst, text, sizeof(text));
    }
    double t_par = now_seconds() - start;

    printf("iterations=%d\n", iterations);
    printf("encode: %.6f s (%.0f ops/s)\n", t_enc, iterations / t_enc);
    printf("decode: %.6f s (%.0f ops/s)\n", t_dec, iterations / t_dec);
    printf("parse : %.6f s (%.0f ops/s)\n", t_par, iterations / t_par);
    return 0;
}
