#include "radio.h"

#include "config.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

static const uint8_t RF_ADDR[5] = { RF_ADDRESS_BYTES };

static uint8_t power_level(void) {
    switch (PA_LEVEL) {
        case 0: return NRF24_POWER_0;
        case 2: return NRF24_POWER_2;
        case 3: return NRF24_POWER_3;
        default: return NRF24_POWER_1;
    }
}

static uint8_t speed_level(void) {
    if (DATA_RATE == 0.25) return NRF24_SPEED_250K;
    if (DATA_RATE == 2) return NRF24_SPEED_2M;
    return NRF24_SPEED_1M;
}

void radio_init(radio_t *r) {
    spi_inst_t *spi = (SPI_ID == 0) ? spi0 : spi1;
    r->spi = spi;
    spi_init(spi, BAUDRATE);
    spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(SPI_MISO, GPIO_FUNC_SPI);

    r->csn_pin = NRF_CSN;
    r->ce_pin = NRF_CE;

    nrf24_init(&r->nrf, spi, NRF_CSN, NRF_CE, RF_CHANNEL, PAYLOAD_SIZE);
    nrf24_set_power_speed(&r->nrf, power_level(), speed_level());
    if (!AUTO_ACK) {
        nrf24_reg_write(&r->nrf, 0x01, 0x00); /* EN_AA disable */
    }
    nrf24_open_tx_pipe(&r->nrf, RF_ADDR);
    nrf24_open_rx_pipe(&r->nrf, 0, RF_ADDR);
    nrf24_start_listening(&r->nrf);
    r->seq = 0;
}

uint16_t radio_next_seq(radio_t *r) {
    r->seq = (uint16_t)((r->seq + 1) & 0xFFFF);
    return r->seq;
}

bool radio_send(radio_t *r, const uint8_t *packet) {
    nrf24_stop_listening(&r->nrf);
    int ok = nrf24_send(&r->nrf, packet, 100);
    nrf24_start_listening(&r->nrf);
    return ok ? true : false;
}

int radio_poll(radio_t *r, uint8_t *out) {
    if (!nrf24_any(&r->nrf)) return 0;
    nrf24_recv(&r->nrf, out);
    return 1;
}
