import time
import config
from ping import protocol
from ping.radio import Radio

radio = Radio()
seen_count = 0
last_report = time.ticks_ms()
print("DEBUG RX node", config.NODE_ID)
while True:
    raw = radio.poll()
    if raw is not None:
        seen_count += 1
        try:
            msg = protocol.decode(raw)
            print("rx", msg["src"], msg["seq"], "text=", msg["text"])
        except ValueError as exc:
            print("bad packet", exc)
    now = time.ticks_ms()
    if time.ticks_diff(now, last_report) >= 1000:
        last_report = now
        try:
            fs = radio.nrf.reg_read(0x17)
            st = radio.nrf.read_status()
            print("health any fifo=0x%02x status=0x%02x count=%d" % (fs, st, seen_count))
        except Exception as exc:
            print("health error", repr(exc))
    time.sleep_ms(5)
