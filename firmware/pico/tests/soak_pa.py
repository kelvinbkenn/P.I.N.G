import time
import config
config.PA_LEVEL = 1
from ping import protocol
from ping.radio import Radio

r = Radio()
count = 0
n = 15
for i in range(1, n + 1):
    seq = r.next_seq()
    ok = r.send(protocol.text_message(config.NODE_ID, 1, seq, "pa1 %d" % i))
    count += int(ok)
    print("pa1 %d/%d %s; acks=%d" % (i, n, "ACK" if ok else "NOACK", count))
    time.sleep_ms(1000)
print("SOAK_PA RESULT %d/%d" % (count, n))
