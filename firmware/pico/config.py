"""Node configuration. Edit per device before flashing."""

# Identity
NODE_ID = 1

# Radio (nRF24L01+)
RF_CHANNEL = 76
RF_ADDRESS = b"PICO1"
PAYLOAD_SIZE = 32
DATA_RATE = 1  # Mbps: 0.25, 1 or 2
PA_LEVEL = 3  # 0 = -18 dBm ... 3 = 0 dBm
AUTO_ACK = True

# IRQ is optional and unused in Phase 1. Reserved for Phase 2.
NRF_IRQ = 21
USE_IRQ = False

# SPI0
SPI_ID = 0
SPI_SCK = 18
SPI_MOSI = 19
SPI_MISO = 16
NRF_CSN = 17
NRF_CE = 20
BAUDRATE = 4_000_000

# OLED (SSD1306 over I2C0)
OLED_I2C_ID = 0
OLED_SDA = 4
OLED_SCL = 5
OLED_ADDR = 0x3C
OLED_WIDTH = 128
OLED_HEIGHT = 64
