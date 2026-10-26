"""Print every packet the radio receives.

Run with: mpremote run firmware/pico/tests/radio_rx_test.py
"""

import time

import config
from ping import protocol
from ping.radio import Radio

radio = Radio()
print("Radio RX test on node", config.NODE_ID, "- Ctrl-C to stop")
while True:
    raw = radio.poll()
    if raw is not None:
        try:
            msg = protocol.decode(raw)
        except ValueError as exc:
            print("bad packet:", exc)
        else:
            print(
                "rx from",
                msg["src"],
                "type",
                msg["type"],
                "seq",
                msg["seq"],
                "text",
                msg["text"],
            )
    time.sleep_ms(5)
