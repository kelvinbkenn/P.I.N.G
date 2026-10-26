# Hardware

## Bill of materials

- Raspberry Pi Pico 2 (RP2350, non-wireless)
- nRF24L01+ module (SPI)
- 0.96" OLED, SSD1306, 128x64, I2C
- ~10 uF capacitor across the radio's 3.3V/GND

## Wiring

| nRF24L01+ | Pico 2 | Notes |
|---|---|---|
| VCC | 3V3 | Do not use 5V |
| GND | GND | |
| MISO | GP16 | SPI0 |
| SCK | GP18 | SPI0 |
| MOSI | GP19 | SPI0 |
| CSN | GP17 | |
| CE | GP20 | |
| IRQ | GP21 | Reserved; **not connected in Phase 1** |

| SSD1306 | Pico 2 | Notes |
|---|---|---|
| VCC | 3V3 | |
| GND | GND | |
| SDA | GP4 | I2C0 |
| SCL | GP5 | I2C0 |

## Notes

- Both the Pico 2 and the radio run at 3.3V.
- Fit the decoupling capacitor close to the nRF24 module; most "flaky link" reports are
  power-related.
- The IRQ pin is optional. Phase 1 polls the radio status register. Connect IRQ on GP21
  when moving to Phase 2.
- Pins are defined in `firmware/pico/config.py`.
