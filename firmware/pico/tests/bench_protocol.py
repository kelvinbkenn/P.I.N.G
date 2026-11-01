"""On-device MicroPython protocol benchmark. Run with mpremote run."""
import time

from ping import protocol

ITERS = 10000
packet = protocol.text_message(1, protocol.BROADCAST, 42, "hello world")
label = "@3 hello world"

def bench(fn, iters=ITERS):
    start = time.ticks_us()
    for _ in range(iters):
        fn()
    return time.ticks_diff(time.ticks_us(), start)

t = bench(lambda: protocol.text_message(1, protocol.BROADCAST, 42, "hello world"))
print("encode us/iter: %f" % (t / ITERS))

t = bench(lambda: protocol.decode(packet))
print("decode us/iter: %f" % (t / ITERS))

t = bench(lambda: protocol.parse_command(label))
print("parse  us/iter: %f" % (t / ITERS))
