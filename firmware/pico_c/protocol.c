#include "protocol.h"

#include <string.h>

static int utf8_valid(const uint8_t *data, size_t len) {
    size_t i = 0;
    while (i < len) {
        uint8_t c = data[i];
        size_t adv;
        if (c < 0x80) {
            adv = 1;
        } else if ((c & 0xE0) == 0xC0) {
            adv = 2;
        } else if ((c & 0xF0) == 0xE0) {
            adv = 3;
        } else if ((c & 0xF8) == 0xF0) {
            adv = 4;
        } else {
            return 0;
        }
        if (i + adv > len) {
            return 0;
        }
        for (size_t j = 1; j < adv; j++) {
            if ((data[i + j] & 0xC0) != 0x80) {
                return 0;
            }
        }
        if (adv == 2 && (c & 0x1E) == 0) return 0;
        i += adv;
    }
    return 1;
}

int ping_encode(uint8_t type, uint8_t src, uint8_t dst, uint16_t seq,
                const void *payload, size_t payload_len, uint8_t flags,
                uint8_t out[PING_PAYLOAD_SIZE]) {
    if (payload_len > PING_MAX_TEXT) {
        return -1;
    }
    memset(out, 0, PING_PAYLOAD_SIZE);
    out[0] = PING_PROTOCOL_VERSION;
    out[1] = type & 0xFF;
    out[2] = src & 0xFF;
    out[3] = dst & 0xFF;
    out[4] = seq & 0xFF;
    out[5] = (seq >> 8) & 0xFF;
    out[6] = flags & 0xFF;
    out[7] = (uint8_t)payload_len;
    if (payload != NULL && payload_len > 0) {
        memcpy(out + PING_HEADER_SIZE, payload, payload_len);
    }
    return 0;
}

int ping_text_message(uint8_t src, uint8_t dst, uint16_t seq, const char *text,
                      uint8_t flags, uint8_t out[PING_PAYLOAD_SIZE]) {
    size_t len = text ? strlen(text) : 0;
    return ping_encode(PING_TYPE_TEXT, src, dst, seq, text, len, flags, out);
}

int ping_decode(const void *data, size_t len, ping_message_t *out) {
    uint8_t buf[PING_PAYLOAD_SIZE];
    memset(buf, 0, PING_PAYLOAD_SIZE);
    if (len > PING_PAYLOAD_SIZE) {
        return -1;
    }
    if (data != NULL && len > 0) {
        memcpy(buf, data, len);
    }
    if (buf[0] != PING_PROTOCOL_VERSION) {
        return -1;
    }
    uint8_t length = buf[7];
    if (length > PING_MAX_TEXT) {
        return -1;
    }
    out->version = buf[0];
    out->type = buf[1];
    out->src = buf[2];
    out->dst = buf[3];
    out->seq = (uint16_t)buf[4] | ((uint16_t)buf[5] << 8);
    out->flags = buf[6];
    out->length = length;
    memset(out->payload, 0, PING_MAX_TEXT);
    memset(out->text, 0, PING_MAX_TEXT + 1);
    if (length > 0) {
        memcpy(out->payload, buf + PING_HEADER_SIZE, length);
    }
    if (length == 0 || utf8_valid(out->payload, length)) {
        if (length > 0) {
            memcpy(out->text, out->payload, length);
        }
        out->text[length] = '\0';
    } else {
        out->text[0] = '\0';
    }
    return 0;
}

static int is_space(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

int ping_parse_command(const char *line, uint8_t *dst, char *text, size_t text_cap) {
    if (line == NULL) return 0;
    size_t start = 0;
    while (line[start] && is_space((unsigned char)line[start])) start++;
    if (line[start] == '\0') {
        return 0;
    }
    if (line[start] == '@') {
        size_t i = start + 1;
        if (line[i] == '\0') return 0;
        size_t num_start = i;
        while (line[i] && (line[i] >= '0' && line[i] <= '9')) i++;
        if (i == num_start) return 0;
        if (!is_space((unsigned char)line[i])) return 0;
        char numbuf[16];
        size_t nlen = i - num_start;
        if (nlen >= sizeof(numbuf)) return 0;
        memcpy(numbuf, line + num_start, nlen);
        numbuf[nlen] = '\0';
        long val = 0;
        for (size_t k = 0; k < nlen; k++) {
            val = val * 10 + (numbuf[k] - '0');
            if (val > 0xFF) return 0;
        }
        size_t j = i;
        while (line[j] && is_space((unsigned char)line[j])) j++;
        if (line[j] == '\0') return 0;
        size_t tlen = strlen(line + j);
        while (tlen > 0 && is_space((unsigned char)line[j + tlen - 1])) tlen--;
        if (tlen == 0) return 0;
        if (tlen >= text_cap) return 0;
        memcpy(text, line + j, tlen);
        text[tlen] = '\0';
        if (dst) *dst = (uint8_t)val;
        return 1;
    }
    size_t j = start;
    size_t tlen = strlen(line + j);
    while (tlen > 0 && is_space((unsigned char)line[j + tlen - 1])) tlen--;
    if (tlen == 0) return 0;
    if (tlen >= text_cap) return 0;
    memcpy(text, line + j, tlen);
    text[tlen] = '\0';
    if (dst) *dst = PING_BROADCAST;
    return 1;
}
