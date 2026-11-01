# P.I.N.G

A private, decentralized internet made using Raspberry Pi Pico and nRF24l01+.
The network supports text messages and voice calls.

## Status

Phase 1: text messaging over nRF24L01+ with an OLED display, written in MicroPython
for the Raspberry Pi Pico 2.

## Hardware

- Raspberry Pi Pico 2 (RP2350)
- nRF24L01+ radio (SPI0)
- 0.96" SSD1306 128x64 OLED (I2C0)

Wiring is in [docs/hardware.md](docs/hardware.md). The plan is in
[docs/roadmap.md](docs/roadmap.md) and [docs/protocol.md](docs/protocol.md).

## Repository layout

```
firmware/pico/        MicroPython firmware for one node
  boot.py, main.py    entrypoints
  ping/               application modules
  lib/                vendored drivers (nrf24l01, ssd1306)
  tests/              on-device bring-up scripts
docs/                 hardware, protocol and roadmap notes
tests/                host-side pytest suite
```

## Development

Host tools (for tests and formatting):

```
python -m pytest
ruff format .
ruff check .
```

Deploy the firmware to a Pico 2 running MicroPython:

```
mpremote fs cp firmware/pico/boot.py :
mpremote fs cp firmware/pico/main.py :
mpremote fs cp -r firmware/pico/ping :
mpremote fs cp -r firmware/pico/lib :
```

Run the app (Ctrl-C to stop; the USB REPL is used for text input):

```
mpremote run firmware/pico/main.py
```

Bring-up scripts can be run directly after the files above are deployed:

```
mpremote run firmware/pico/tests/i2c_scan.py
mpremote run firmware/pico/tests/oled_hello.py
mpremote run firmware/pico/tests/radio_tx_test.py
mpremote run firmware/pico/tests/radio_rx_test.py
```

Set `NODE_ID` in `firmware/pico/config.py` before flashing each node.

## C port

A C port targeting the Raspberry Pi Pico SDK lives in `firmware/pico_c/`. It mirrors the
modules in `firmware/pico/` (protocol, radio, display, buzzer, console) and uses the
same wire protocol. Configure pins/RF settings in `firmware/pico_c/config.h`. Build with
the Pico SDK (set `PICO_SDK_PATH`):

```
cd firmware/pico_c
mkdir -p build && cd build
cmake ..
make
```

The host-testable protocol translation can be built and run with:

```
cd firmware/pico_c
make test-protocol
```

Protocol performance (host, GCC -O2):

```
cd firmware/pico_c
make bench-protocol
```

MicroPython on-device protocol benchmark:

```
mpremote run firmware/pico/tests/bench_protocol.py
```
On-device C benchmark metrics are printed to USB serial every 5000 loop iterations
when `BENCHMARK` is enabled in `firmware/pico_c/main.c`.