"""Measure nRF24L01+ round-trip RTT and show it on the OLED for demos.

Run on one board while another board runs radio_rx_test.py. The sending board's
OLED updates after each packet.
"""

import time

import config
from ping import protocol
from ping.display import Display
from ping.radio import Radio

target = 2 if config.NODE_ID == 1 else 1
samples = 20
radio = Radio()
display = Display()
oled = display.oled


def show(lines):
    oled.fill(0)
    y = 0
    for line in lines[:6]:
        oled.text(line[:16], 0, y)
        y += 11
    oled.show()


show(["LATENCY TEST", "node %d -> %d" % (config.NODE_ID, target), "", "starting..."])
time.sleep_ms(500)

latencies = []
ok_count = 0
for n in range(1, samples + 1):
    seq = radio.next_seq()
    packet = protocol.text_message(config.NODE_ID, target, seq, "ping %d" % n)
    start = time.ticks_ms()
    ok = radio.send(packet)
    elapsed = time.ticks_diff(time.ticks_ms(), start)
    latencies.append((ok, elapsed))
    if ok:
        ok_count += 1
    print("%d seq=%d %s %d ms" % (n, seq, "ok" if ok else "no-ack", elapsed))
    successful = [e for state, e in latencies if state]
    avg = (sum(successful) // len(successful)) if successful else 0
    show(
        [
            "LATENCY TEST",
            "n%d -> n%d" % (config.NODE_ID, target),
            "last %3d ms %s" % (elapsed, "ok" if ok else "FAIL"),
            "avg  %3d ms" % avg,
            "min  %3d ms" % (min(successful) if successful else 0),
            "max  %3d ms" % (max(successful) if successful else 0),
        ]
    )
    time.sleep_ms(250)

successful = [e for state, e in latencies if state]
avg = (sum(successful) // len(successful)) if successful else 0
show(
    [
        "RESULT",
        "n%d -> n%d" % (config.NODE_ID, target),
        "%d/%d acked" % (ok_count, samples),
        "avg %d ms" % avg,
        "min %d max %d" % (min(successful) if successful else 0, max(successful) if successful else 0),
        "ready",
    ]
)
print(
    "done: %d/%d acked, avg=%d ms, min=%d ms, max=%d ms"
    % (ok_count, samples, avg, min(successful) if successful else 0, max(successful) if successful else 0)
)
