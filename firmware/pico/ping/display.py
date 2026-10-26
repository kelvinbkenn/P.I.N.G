"""OLED rendering for P.I.N.G."""

from machine import I2C, Pin

import config
import ssd1306


class Display:
    def __init__(self):
        i2c = I2C(
            config.OLED_I2C_ID,
            sda=Pin(config.OLED_SDA),
            scl=Pin(config.OLED_SCL),
            freq=400_000,
        )
        self.oled = ssd1306.SSD1306_I2C(
            config.OLED_WIDTH, config.OLED_HEIGHT, i2c, addr=config.OLED_ADDR
        )
        self.lines = []
        self.show_status("P.I.N.G", "booting")

    def show_status(self, line1, line2=""):
        oled = self.oled
        oled.fill(0)
        oled.text(line1[:16], 0, 0)
        if line2:
            oled.text(line2[:16], 0, 12)
        oled.show()

    def add_message(self, label, text):
        self.lines.append("%s %s" % (label, text))
        if len(self.lines) > 32:
            self.lines = self.lines[-32:]
        self._render()

    def _render(self):
        oled = self.oled
        oled.fill(0)
        oled.text("P.I.N.G", 0, 0)
        oled.hline(0, 10, config.OLED_WIDTH, 1)
        y = 14
        for line in self.lines[-5:]:
            oled.text(line[:16], 0, y)
            y += 10
        oled.show()
