# Performance comparison: MicroPython vs C

Keep both firmware tracks while comparing. Remove MicroPython only after the C port
passes hardware smoke tests and the numbers below are captured.

## What can be compared

1. Protocol processing (host):
   - C: `cd firmware/pico_c && make bench-protocol`
   - MicroPython/C Python baseline: `python -m pytest` is not a benchmark; use
     `firmware/pico/tests/bench_protocol.py` on a board for MicroPython timing.
2. End-to-end loop rate:
   - C: enable `BENCHMARK` in `firmware/pico_c/main.c` and read USB serial.
   - MicroPython: port the same loop counter into a temporary copy of `main.py`.
3. Radio latency and throughput:
   - Use `radio_tx_test.py`, `radio_rx_test.py`, and `radio_latency_test.py` on
     MicroPython boards, and equivalent traffic on C boards.
4. OLED refresh cost:
   - Time `display_add_message()` on both stacks.

## Fairness notes

- Match RF settings, PA level, payload size, and channel exactly.
- Use the same loop delay (5 ms) and dedupe limit.
- Disable auto-ack only for raw throughput tests; otherwise keep `AUTO_ACK` on.
- Record board temperature/power and PA level because most link differences are
  RF/power related, not MCU language related.

## Expected reporting

Record median and worst-case latency, loop iterations per second, and send success
rate for each stack in the same table before removing MicroPython.
