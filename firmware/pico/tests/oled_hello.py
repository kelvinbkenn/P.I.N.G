"""Draw a greeting on the OLED.

Run with: mpremote run firmware/pico/tests/oled_hello.py
"""

from machine import I2C, Pin

import config
import ssd1306

i2c = I2C(
    config.OLED_I2C_ID,
    sda=Pin(config.OLED_SDA),
    scl=Pin(config.OLED_SCL),
    freq=400_000,
)
oled = ssd1306.SSD1306_I2C(config.OLED_WIDTH, config.OLED_HEIGHT, i2c, addr=config.OLED_ADDR)
oled.fill(0)
oled.text("P.I.N.G", 0, 0)
oled.text("OLED OK", 0, 16)
oled.show()
print("OLED updated")
