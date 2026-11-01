#include "nrf24.h"

#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

static void csn_low(nrf24_t *n) {
    gpio_put(n->csn_pin, 0);
}

static void csn_high(nrf24_t *n) {
    gpio_put(n->csn_pin, 1);
}

static void ce_low(nrf24_t *n) {
    gpio_put(n->ce_pin, 0);
}

static void ce_high(nrf24_t *n) {
    gpio_put(n->ce_pin, 1);
}

static uint8_t spi_transfer_one(nrf24_t *n, uint8_t value) {
    uint8_t tx = value;
    uint8_t rx = 0;
    spi_write_read_blocking(n->spi, &tx, &rx, 1);
    return rx;
}

uint8_t nrf24_reg_read(nrf24_t *n, uint8_t reg) {
    uint8_t tx[2] = { reg, 0xFF };
    uint8_t rx[2] = { 0, 0 };
    csn_low(n);
    spi_write_read_blocking(n->spi, tx, rx, 2);
    csn_high(n);
    return rx[1];
}

void nrf24_reg_write(nrf24_t *n, uint8_t reg, uint8_t value) {
    uint8_t tx[2] = { (uint8_t)(0x20 | reg), value };
    uint8_t rx[2] = { 0, 0 };
    csn_low(n);
    spi_write_read_blocking(n->spi, tx, rx, 2);
    csn_high(n);
}

static void reg_write_bytes(nrf24_t *n, uint8_t reg, const uint8_t *buf, size_t len) {
    uint8_t cmd = (uint8_t)(0x20 | reg);
    csn_low(n);
    spi_write_blocking(n->spi, &cmd, 1);
    if (len > 0) {
        spi_write_blocking(n->spi, buf, len);
    }
    csn_high(n);
}

static void flush_rx(nrf24_t *n) {
    uint8_t cmd = NRF24_CMD_FLUSH_RX;
    csn_low(n);
    spi_write_blocking(n->spi, &cmd, 1);
    csn_high(n);
}

static void flush_tx(nrf24_t *n) {
    uint8_t cmd = NRF24_CMD_FLUSH_TX;
    csn_low(n);
    spi_write_blocking(n->spi, &cmd, 1);
    csn_high(n);
}

void nrf24_set_power_speed(nrf24_t *n, uint8_t power, uint8_t speed) {
    uint8_t setup = nrf24_reg_read(n, NRF24_REG_RF_SETUP) & 0xD1;
    nrf24_reg_write(n, NRF24_REG_RF_SETUP, (uint8_t)(setup | power | speed));
}

void nrf24_open_tx_pipe(nrf24_t *n, const uint8_t *address) {
    reg_write_bytes(n, NRF24_REG_RX_ADDR_P0, address, 5);
    reg_write_bytes(n, NRF24_REG_TX_ADDR, address, 5);
    nrf24_reg_write(n, NRF24_REG_RX_PW_P0, n->payload_size);
}

void nrf24_open_rx_pipe(nrf24_t *n, uint8_t pipe_id, const uint8_t *address) {
    if (pipe_id == 0) {
        memcpy(n->pipe0_addr, address, 5);
        n->has_pipe0 = 1;
    }
    if (pipe_id < 2) {
        reg_write_bytes(n, (uint8_t)(NRF24_REG_RX_ADDR_P0 + pipe_id), address, 5);
    } else {
        nrf24_reg_write(n, (uint8_t)(NRF24_REG_RX_ADDR_P0 + pipe_id), address[0]);
    }
    nrf24_reg_write(n, (uint8_t)(NRF24_REG_RX_PW_P0 + pipe_id), n->payload_size);
    uint8_t en = nrf24_reg_read(n, NRF24_REG_EN_RXADDR);
    nrf24_reg_write(n, NRF24_REG_EN_RXADDR, (uint8_t)(en | (1u << pipe_id)));
}

void nrf24_start_listening(nrf24_t *n) {
    nrf24_reg_write(n, NRF24_REG_CONFIG,
                    (uint8_t)(nrf24_reg_read(n, NRF24_REG_CONFIG) | NRF24_PWR_UP | NRF24_PRIM_RX));
    nrf24_reg_write(n, NRF24_REG_STATUS,
                    (uint8_t)(NRF24_RX_DR | NRF24_TX_DS | NRF24_MAX_RT));
    if (n->has_pipe0) {
        reg_write_bytes(n, NRF24_REG_RX_ADDR_P0, n->pipe0_addr, 5);
    }
    flush_rx(n);
    flush_tx(n);
    ce_high(n);
    sleep_us(130);
}

void nrf24_stop_listening(nrf24_t *n) {
    ce_low(n);
    flush_tx(n);
    flush_rx(n);
}

int nrf24_any(nrf24_t *n) {
    uint8_t fifo = nrf24_reg_read(n, NRF24_REG_FIFO_STATUS);
    return !(fifo & NRF24_RX_EMPTY);
}

void nrf24_recv(nrf24_t *n, uint8_t *out) {
    uint8_t cmd = NRF24_CMD_R_RX_PAYLOAD;
    csn_low(n);
    spi_write_blocking(n->spi, &cmd, 1);
    spi_read_blocking(n->spi, 0xFF, out, n->payload_size);
    csn_high(n);
    nrf24_reg_write(n, NRF24_REG_STATUS, NRF24_RX_DR);
}

int nrf24_send(nrf24_t *n, const uint8_t *buf, uint32_t timeout_ms) {
    nrf24_reg_write(n, NRF24_REG_CONFIG,
                    (uint8_t)((nrf24_reg_read(n, NRF24_REG_CONFIG) | NRF24_PWR_UP) & ~NRF24_PRIM_RX));
    sleep_us(1500);

    uint8_t cmd = NRF24_CMD_W_TX_PAYLOAD;
    csn_low(n);
    spi_write_blocking(n->spi, &cmd, 1);
    spi_write_blocking(n->spi, buf, n->payload_size);
    csn_high(n);

    ce_high(n);
    sleep_us(15);
    ce_low(n);

    uint32_t start = to_ms_since_boot(get_absolute_time());
    int result = 0; /* 0 timeout, 1 success, 2 fail */
    while (result == 0) {
        uint8_t status = nrf24_reg_read(n, NRF24_REG_STATUS);
        if (status & (NRF24_TX_DS | NRF24_MAX_RT)) {
            result = (status & NRF24_TX_DS) ? 1 : 2;
            nrf24_reg_write(n, NRF24_REG_STATUS,
                            (uint8_t)(NRF24_RX_DR | NRF24_TX_DS | NRF24_MAX_RT));
            /* power down */
            nrf24_reg_write(n, NRF24_REG_CONFIG,
                            (uint8_t)(nrf24_reg_read(n, NRF24_REG_CONFIG) & ~NRF24_PWR_UP));
            break;
        }
        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - start >= timeout_ms) {
            flush_tx(n);
            nrf24_reg_write(n, NRF24_REG_CONFIG,
                            (uint8_t)(nrf24_reg_read(n, NRF24_REG_CONFIG) & ~NRF24_PWR_UP));
            return 0;
        }
        sleep_ms(1);
    }
    /* ensure power down */
    nrf24_reg_write(n, NRF24_REG_CONFIG,
                    (uint8_t)(nrf24_reg_read(n, NRF24_REG_CONFIG) & ~NRF24_PWR_UP));
    return result == 1 ? 1 : 0;
}

void nrf24_init(nrf24_t *n, spi_inst_t *spi, uint csn_pin, uint ce_pin,
                uint8_t channel, uint8_t payload_size) {
    n->spi = spi;
    n->csn_pin = csn_pin;
    n->ce_pin = ce_pin;
    n->payload_size = payload_size;
    n->channel = channel;
    n->has_pipe0 = 0;

    gpio_init(csn_pin);
    gpio_set_dir(csn_pin, GPIO_OUT);
    gpio_put(csn_pin, 1);
    gpio_init(ce_pin);
    gpio_set_dir(ce_pin, GPIO_OUT);
    gpio_put(ce_pin, 0);

    sleep_ms(5);

    nrf24_reg_write(n, NRF24_REG_SETUP_AW, 0x03);
    /* if (nrf24_reg_read(...) != 0x03) hardware not responding; ignore */

    nrf24_reg_write(n, NRF24_REG_DYNPD, 0x00);
    nrf24_reg_write(n, NRF24_REG_SETUP_RETR, (uint8_t)((6 << 4) | 8));

    /* best default, overridden by caller */
    nrf24_set_power_speed(n, NRF24_POWER_3, NRF24_SPEED_250K);

    /* init CRC length 2 */
    {
        uint8_t cfg = nrf24_reg_read(n, NRF24_REG_CONFIG) & ~(NRF24_CRCO | NRF24_EN_CRC);
        cfg |= (uint8_t)(NRF24_EN_CRC | NRF24_CRCO);
        nrf24_reg_write(n, NRF24_REG_CONFIG, cfg);
    }

    nrf24_reg_write(n, NRF24_REG_STATUS,
                    (uint8_t)(NRF24_RX_DR | NRF24_TX_DS | NRF24_MAX_RT));
    nrf24_reg_write(n, NRF24_REG_RF_CH, (uint8_t)(channel > 125 ? 125 : channel));

    flush_rx(n);
    flush_tx(n);
}
PY
echo "written"; sed -n '1,20p' /tmp/opencode/gen_font.py >/dev/null