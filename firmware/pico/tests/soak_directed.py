import time
import config
from ping import protocol
from ping.radio import Radio

r = Radio()
count = 0
n = 30
for i in range(1, n + 1):
    seq = r.next_seq()
    ok = r.send(protocol.text_message(config.NODE_ID, 1, seq, "soak %d" % i))
    count += int(ok)
    print("soak %d/%d seq=%d %s; acks=%d" % (i, n, seq, "ACK" if ok else "NOACK", count))
    time.sleep_ms(1000)
print("SOAK RESULT %d/%d" % (count, n))
