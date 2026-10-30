"""Measure nRF24L01+ round-trip time using auto-acknowledge (MAC layer).

Run on one board while another board runs radio_rx_test.py. Each send() call blocks
until the radio gets an AUTO_ACK or hits its timeout, so elapsed time is roughly air
plus radio turn-around time, not full application processing.
"""

import time

import config
from ping import protocol
from ping.radio import Radio

target = 2 if config.NODE_ID == 1 else 1
radio = Radio()
samples = 20


def main():
    print("Latency test: node %d -> node %d (%d packets)" % (config.NODE_ID, target, samples))
    ok_count = 0
    total = 0
    latencies = []
    for n in range(1, samples + 1):
        seq = radio.next_seq()
        text = "ping %d" % n
        packet = protocol.text_message(config.NODE_ID, target, seq, text)
        start = time.ticks_ms()
        ok = radio.send(packet)
        elapsed = time.ticks_diff(time.ticks_ms(), start)
        latencies.append((ok, elapsed))
        if ok:
            ok_count += 1
            total += elapsed
        print("%d seq=%d %s %d ms" % (n, seq, "ok" if ok else "no-ack", elapsed))
        time.sleep_ms(250)

    successful = [e for ok, e in latencies if ok]
    print(
        "done: %d/%d acked, avg=%d ms, min=%d ms, max=%d ms"
        % (
            ok_count,
            samples,
            (sum(successful) // len(successful)) if successful else 0,
            min(successful) if successful else 0,
            max(successful) if successful else 0,
        )
    )


main()
