CC ?= gcc
CFLAGS ?= -std=c99 -Wall -Wextra -Werror -O2
CPPFLAGS ?= -I. -Ilib

PROTOCOL_TEST := ../../tests/test_protocol.c
PROTOCOL_BENCH := ../../tests/test_protocol_bench.c

test-protocol:
	$(CC) $(CFLAGS) $(CPPFLAGS) protocol.c $(PROTOCOL_TEST) -o /tmp/ping_protocol_test
	/tmp/ping_protocol_test

bench-protocol:
	$(CC) $(CFLAGS) $(CPPFLAGS) protocol.c $(PROTOCOL_BENCH) -o /tmp/ping_protocol_bench
	/tmp/ping_protocol_bench

clean:
	rm -f /tmp/ping_protocol_test /tmp/ping_protocol_bench
