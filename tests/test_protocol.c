#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "protocol.h"

static void test_round_trip_text(void) {
    uint8_t packet[PING_PAYLOAD_SIZE];
    assert(ping_text_message(1, PING_BROADCAST, 42, "hello", 0, packet) == 0);
    assert((int)sizeof(packet) == PING_PAYLOAD_SIZE);
    ping_message_t msg;
    assert(ping_decode(packet, PING_PAYLOAD_SIZE, &msg) == 0);
    assert(msg.type == PING_TYPE_TEXT);
    assert(msg.src == 1);
    assert(msg.dst == PING_BROADCAST);
    assert(msg.seq == 42);
    assert(strcmp(msg.text, "hello") == 0);
    printf("ok round_trip_text\n");
}

static void test_max_text_length(void) {
    char text[PING_MAX_TEXT + 1];
    for (int i = 0; i < PING_MAX_TEXT; i++) text[i] = 'x';
    text[PING_MAX_TEXT] = '\0';
    uint8_t packet[PING_PAYLOAD_SIZE];
    assert(ping_text_message(2, 1, 1, text, 0, packet) == 0);
    ping_message_t msg;
    assert(ping_decode(packet, PING_PAYLOAD_SIZE, &msg) == 0);
    assert(strcmp(msg.text, text) == 0);
    printf("ok max_text_length\n");
}

static void test_payload_too_long(void) {
    char text[PING_MAX_TEXT + 2];
    for (int i = 0; i < PING_MAX_TEXT + 1; i++) text[i] = 'x';
    text[PING_MAX_TEXT + 1] = '\0';
    uint8_t packet[PING_PAYLOAD_SIZE];
    assert(ping_text_message(1, 2, 1, text, 0, packet) == -1);
    printf("ok payload_too_long\n");
}

static void test_decode_pads_short_packet(void) {
    uint8_t packet[PING_PAYLOAD_SIZE];
    assert(ping_text_message(1, 2, 7, "hi", 0, packet) == 0);
    ping_message_t msg;
    assert(ping_decode(packet, PING_HEADER_SIZE + 2, &msg) == 0);
    assert(strcmp(msg.text, "hi") == 0);
    assert(msg.seq == 7);
    printf("ok decode_pads_short_packet\n");
}

static void test_decode_rejects_wrong_version(void) {
    uint8_t packet[PING_PAYLOAD_SIZE];
    assert(ping_text_message(1, 2, 1, "hi", 0, packet) == 0);
    packet[0] = 99;
    ping_message_t msg;
    assert(ping_decode(packet, PING_PAYLOAD_SIZE, &msg) == -1);
    printf("ok decode_rejects_wrong_version\n");
}

static void test_decode_unicode(void) {
    uint8_t packet[PING_PAYLOAD_SIZE];
    assert(ping_text_message(1, 2, 1, "h\xc3\xa9", 0, packet) == 0);
    ping_message_t msg;
    assert(ping_decode(packet, PING_PAYLOAD_SIZE, &msg) == 0);
    assert(strcmp(msg.text, "h\xc3\xa9") == 0);
    printf("ok decode_unicode\n");
}

static void test_parse_broadcast(void) {
    char text[64];
    uint8_t dst;
    assert(ping_parse_command("hello world", &dst, text, sizeof(text)) == 1);
    assert(dst == PING_BROADCAST);
    assert(strcmp(text, "hello world") == 0);
    printf("ok parse_broadcast\n");
}

static void test_parse_directed(void) {
    char text[64];
    uint8_t dst;
    assert(ping_parse_command("@3 hello", &dst, text, sizeof(text)) == 1);
    assert(dst == 3);
    assert(strcmp(text, "hello") == 0);
    printf("ok parse_directed\n");
}

static void test_parse_empty(void) {
    char text[64];
    uint8_t dst;
    assert(ping_parse_command("   ", &dst, text, sizeof(text)) == 0);
    printf("ok parse_empty\n");
}

static void test_parse_bad_directive(void) {
    char text[64];
    uint8_t dst;
    assert(ping_parse_command("@abc hi", &dst, text, sizeof(text)) == 0);
    assert(ping_parse_command("@9", &dst, text, sizeof(text)) == 0);
    printf("ok parse_bad_directive\n");
}

int main(void) {
    test_round_trip_text();
    test_max_text_length();
    test_payload_too_long();
    test_decode_pads_short_packet();
    test_decode_rejects_wrong_version();
    test_decode_unicode();
    test_parse_broadcast();
    test_parse_directed();
    test_parse_empty();
    test_parse_bad_directive();
    printf("all protocol tests passed\n");
    return 0;
}
