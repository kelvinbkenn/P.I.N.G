"""nRF24L01+ wrapper exposing a simple packet interface."""

from machine import Pin, SPI

import config
import nrf24l01

_POWER = {
    0: nrf24l01.POWER_0,
    1: nrf24l01.POWER_1,
    2: nrf24l01.POWER_2,
    3: nrf24l01.POWER_3,
}
_RATE = {
    0.25: nrf24l01.SPEED_250K,
    1: nrf24l01.SPEED_1M,
    2: nrf24l01.SPEED_2M,
}


class Radio:
    def __init__(self):
        self.spi = SPI(
            config.SPI_ID,
            baudrate=config.BAUDRATE,
            polarity=0,
            phase=0,
            sck=Pin(config.SPI_SCK),
            mosi=Pin(config.SPI_MOSI),
            miso=Pin(config.SPI_MISO),
        )
        self.csn = Pin(config.NRF_CSN, Pin.OUT, value=1)
        self.ce = Pin(config.NRF_CE, Pin.OUT, value=0)
        self.nrf = nrf24l01.NRF24L01(
            self.spi,
            self.csn,
            self.ce,
            channel=config.RF_CHANNEL,
            payload_size=config.PAYLOAD_SIZE,
        )
        self.nrf.set_power_speed(_POWER[config.PA_LEVEL], _RATE[config.DATA_RATE])
        if not config.AUTO_ACK:
            self.nrf.reg_write(0x01, 0x00)  # EN_AA: disable auto-acknowledge
        self.nrf.open_tx_pipe(config.RF_ADDRESS)
        self.nrf.open_rx_pipe(0, config.RF_ADDRESS)
        self.nrf.start_listening()
        self._seq = 0

    def next_seq(self):
        self._seq = (self._seq + 1) & 0xFFFF
        return self._seq

    def send(self, packet):
        self.nrf.stop_listening()
        try:
            self.nrf.send(packet, timeout=100)
            return True
        except OSError:
            return False
        finally:
            self.nrf.start_listening()

    def poll(self):
        if not self.nrf.any():
            return None
        return self.nrf.recv()
