#ifndef PING_CONFIG_H
#define PING_CONFIG_H

#define NODE_ID 1

#define RF_CHANNEL 76
#define RF_ADDRESS "PICO1"
#define RF_ADDRESS_BYTES 0x50, 0x49, 0x43, 0x4F, 0x31
#define PAYLOAD_SIZE 32
#define DATA_RATE 1 /* Mbps: 0.25, 1 or 2 */
#define PA_LEVEL 1  /* 0=-18dBm ... 3=0dBm */
#define AUTO_ACK 1

#define NRF_IRQ 21
#define USE_IRQ 0

#define SPI_ID 1 /* spi1 */
#define SPI_SCK 10
#define SPI_MOSI 11
#define SPI_MISO 12
#define NRF_CSN 13
#define NRF_CE 14
#define BAUDRATE 4000000

#define OLED_I2C_ID 0
#define OLED_SDA 4
#define OLED_SCL 5
#define OLED_ADDR 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define BUZZER_PIN 2

#endif /* PING_CONFIG_H */
