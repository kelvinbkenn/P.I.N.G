"""Node configuration. Edit per device before flashing."""

# Identity
NODE_ID = 1

# Radio (nRF24L01+)
RF_CHANNEL = 76
RF_ADDRESS = b"PICO1"
PAYLOAD_SIZE = 32
DATA_RATE = 1  # Mbps: 0.25, 1 or 2
PA_LEVEL = 1  # 0 = -18 dBm ... 3 = 0 dBm (use 1 for close boards)
AUTO_ACK = True

# IRQ is optional and unused in Phase 1. Reserved for Phase 2.
NRF_IRQ = 21
USE_IRQ = False

# SPI1 (user wired to GP10/GP11/GP12; CSN/CE on GP13/GP14)
SPI_ID = 1
SPI_SCK = 10
SPI_MOSI = 11
SPI_MISO = 12
NRF_CSN = 13
NRF_CE = 14
BAUDRATE = 4_000_000

# OLED (SSD1306 over I2C0)
OLED_I2C_ID = 0
OLED_SDA = 4
OLED_SCL = 5
OLED_ADDR = 0x3C
OLED_WIDTH = 128
BUZZER_PIN = 2
