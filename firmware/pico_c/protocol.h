#ifndef PING_PROTOCOL_H
#define PING_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#define PING_PROTOCOL_VERSION 1
#define PING_PAYLOAD_SIZE 32
#define PING_HEADER_SIZE 8
#define PING_MAX_TEXT 24
#define PING_BROADCAST 0xFF

enum ping_packet_type {
    PING_TYPE_TEXT = 1,
    PING_TYPE_ACK = 2,
    PING_TYPE_BEACON = 3,
    PING_TYPE_AUDIO = 4,
};

typedef struct {
    uint8_t version;
    uint8_t type;
    uint8_t src;
    uint8_t dst;
    uint16_t seq;
    uint8_t flags;
    uint8_t length;
    uint8_t payload[PING_MAX_TEXT];
    char text[PING_MAX_TEXT + 1];
} ping_message_t;

int ping_encode(uint8_t type, uint8_t src, uint8_t dst, uint16_t seq,
                const void *payload, size_t payload_len, uint8_t flags,
                uint8_t out[PING_PAYLOAD_SIZE]);

int ping_text_message(uint8_t src, uint8_t dst, uint16_t seq, const char *text,
                      uint8_t flags, uint8_t out[PING_PAYLOAD_SIZE]);

int ping_decode(const void *data, size_t len, ping_message_t *out);

int ping_parse_command(const char *line, uint8_t *dst, char *text, size_t text_cap);

#endif /* PING_PROTOCOL_H */
