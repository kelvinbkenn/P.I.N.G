"""Simple active/piezo buzzer helper over one GPIO."""

import time

import config
from machine import Pin


class Buzzer:
    def __init__(self):
        self.pin = Pin(config.BUZZER_PIN, Pin.OUT, value=0)

    def beep(self, count=1, on_ms=80, off_ms=80):
        for index in range(count):
            self.pin.on()
            time.sleep_ms(on_ms)
            self.pin.off()
            if index + 1 < count:
                time.sleep_ms(off_ms)
