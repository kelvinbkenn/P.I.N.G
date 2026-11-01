#ifndef PING_RADIO_H
#define PING_RADIO_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"
#include "nrf24.h"

typedef struct {
    void *spi; /* spi_inst_t* */
    int csn_pin;
    int ce_pin;
    uint16_t seq;
    nrf24_t nrf;
} radio_t;

void radio_init(radio_t *r);
uint16_t radio_next_seq(radio_t *r);
bool radio_send(radio_t *r, const uint8_t *packet);
/* returns 1 if a packet was read into out (PAYLOAD_SIZE bytes) */
int radio_poll(radio_t *r, uint8_t *out);

#endif /* PING_RADIO_H */
