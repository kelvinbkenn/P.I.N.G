"""Host CPython protocol benchmark mirroring tests/test_protocol_bench.c."""
import time

import sys
sys.path.insert(0, "firmware/pico")
from ping import protocol

ITERS = 1_000_000
packet = protocol.text_message(1, protocol.BROADCAST, 42, "hello world")


def bench(func):
    start = time.perf_counter()
    for _ in range(ITERS):
        func()
    return time.perf_counter() - start


t = bench(lambda: protocol.text_message(1, protocol.BROADCAST, 42, "hello world"))
print(f"encode: {t:.6f} s ({ITERS / t:.0f} ops/s)")

t = bench(lambda: protocol.decode(packet))
print(f"decode: {t:.6f} s ({ITERS / t:.0f} ops/s)")

t = bench(lambda: protocol.parse_command("@3 hello world"))
print(f"parse : {t:.6f} s ({ITERS / t:.0f} ops/s)")
