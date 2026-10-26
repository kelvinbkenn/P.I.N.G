"""Send a numbered test packet once per second.

Run with: mpremote run firmware/pico/tests/radio_tx_test.py
"""

import time

import config
from ping import protocol
from ping.radio import Radio

radio = Radio()
print("Radio TX test on node", config.NODE_ID, "- Ctrl-C to stop")
seq = 0
while True:
    seq = (seq + 1) & 0xFFFF
    packet = protocol.text_message(config.NODE_ID, protocol.BROADCAST, seq, "ping %d" % seq)
    ok = radio.send(packet)
    print("sent", seq, "ok" if ok else "no ack")
    time.sleep_ms(1000)
