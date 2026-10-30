import time
import config
from ping import protocol
from ping.radio import Radio

radio = Radio()
for n in range(1, 7):
    seq = (n) & 0xFFFF
    seq = radio.next_seq()
    packet = protocol.text_message(config.NODE_ID, 1, seq, "direct %d" % n)
    ok = radio.send(packet)
    print("direct", n, "seq", seq, "ACK" if ok else "NOACK")
    time.sleep_ms(1000)
print("done")
