#ifndef PING_NRF24_H
#define PING_NRF24_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "hardware/spi.h"

#define NRF24_POWER_0 0x00
#define NRF24_POWER_1 0x02
#define NRF24_POWER_2 0x04
#define NRF24_POWER_3 0x06

#define NRF24_SPEED_250K 0x20
#define NRF24_SPEED_1M 0x00
#define NRF24_SPEED_2M 0x08

#define NRF24_EN_CRC 0x08
#define NRF24_CRCO 0x04
#define NRF24_PWR_UP 0x02
#define NRF24_PRIM_RX 0x01
#define NRF24_RX_DR 0x40
#define NRF24_TX_DS 0x20
#define NRF24_MAX_RT 0x10

#define NRF24_REG_CONFIG 0x00
#define NRF24_REG_EN_RXADDR 0x02
#define NRF24_REG_SETUP_AW 0x03
#define NRF24_REG_SETUP_RETR 0x04
#define NRF24_REG_RF_CH 0x05
#define NRF24_REG_RF_SETUP 0x06
#define NRF24_REG_STATUS 0x07
#define NRF24_REG_RX_ADDR_P0 0x0A
#define NRF24_REG_TX_ADDR 0x10
#define NRF24_REG_RX_PW_P0 0x11
#define NRF24_REG_FIFO_STATUS 0x17
#define NRF24_REG_DYNPD 0x1C

#define NRF24_CMD_R_RX_PAYLOAD 0x61
#define NRF24_CMD_W_TX_PAYLOAD 0xA0
#define NRF24_CMD_FLUSH_TX 0xE1
#define NRF24_CMD_FLUSH_RX 0xE2
#define NRF24_CMD_NOP 0xFF

#define NRF24_RX_EMPTY 0x01

typedef struct {
    spi_inst_t *spi;
    uint csn_pin;
    uint ce_pin;
    uint8_t payload_size;
    uint8_t channel;
    uint8_t pipe0_addr[5];
    int has_pipe0;
} nrf24_t;

void nrf24_init(nrf24_t *n, spi_inst_t *spi, uint csn_pin, uint ce_pin,
                uint8_t channel, uint8_t payload_size);
void nrf24_set_power_speed(nrf24_t *n, uint8_t power, uint8_t speed);
void nrf24_reg_write(nrf24_t *n, uint8_t reg, uint8_t value);
uint8_t nrf24_reg_read(nrf24_t *n, uint8_t reg);
void nrf24_open_tx_pipe(nrf24_t *n, const uint8_t *address);
void nrf24_open_rx_pipe(nrf24_t *n, uint8_t pipe_id, const uint8_t *address);
void nrf24_start_listening(nrf24_t *n);
void nrf24_stop_listening(nrf24_t *n);
int nrf24_any(nrf24_t *n);
void nrf24_recv(nrf24_t *n, uint8_t *out);
int nrf24_send(nrf24_t *n, const uint8_t *buf, uint32_t timeout_ms);

#endif /* PING_NRF24_H */
