"""Scan the OLED I2C bus.

Run with: mpremote run firmware/pico/tests/i2c_scan.py
"""

from machine import I2C, Pin

import config

i2c = I2C(
    config.OLED_I2C_ID,
    sda=Pin(config.OLED_SDA),
    scl=Pin(config.OLED_SCL),
    freq=400_000,
)
found = i2c.scan()
print("I2C devices:", [hex(addr) for addr in found])
if config.OLED_ADDR in found:
    print("OLED found at", hex(config.OLED_ADDR))
else:
    print("OLED not found; expected", hex(config.OLED_ADDR))
