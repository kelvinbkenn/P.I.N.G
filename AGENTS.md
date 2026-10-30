# P.I.N.G — agent notes

Embedded hardware project: Raspberry Pi Pico 2 (RP2350) + nRF24L01+, MicroPython
firmware, SSD1306 128x64 OLED. No compile or package step; firmware is copied to
the board.

## Commands

- Host tests (protocol only, no hardware): `python -m pytest`
  Focus: `python -m pytest tests/test_protocol.py -k <name>`
- Format/lint, in this order: `ruff format .` then `ruff check .`
  Config: `pyproject.toml` (line length 100, Python 3.9; `firmware/pico/lib/**` excluded).
- Deploy to a MicroPython board:
  `mpremote fs cp firmware/pico/boot.py :`
  `mpremote fs cp firmware/pico/main.py :`
  `mpremote fs cp -r firmware/pico/ping :`
  `mpremote fs cp -r firmware/pico/lib :`
- Run on board (Ctrl-C to stop): `mpremote run firmware/pico/main.py`
- On-device bring-up: `mpremote run firmware/pico/tests/<script>.py`
- There is no build system, package manifest, or CI. Do not invent `npm`/`pip install`/`make` steps.

## Boundaries

- `firmware/pico/ping/protocol.py` must stay pure Python for CPython and MicroPython:
  it is the only host-tested module. Do not add `config`, `machine`, or vendored-driver
  imports to it.
- Other app modules import top-level `config`, `nrf24l01`, and `ssd1306`; these are
  device-only and cannot be imported by host tests.
- `firmware/pico/lib/` is vendored third-party; do not edit.
- `firmware/pico/tests/` are on-device scripts, not pytest.
- `tests/` is the host pytest suite; `conftest.py` adds `firmware/pico` to `sys.path`.

## Configuration

- Pins, RF channel/address, payload size, and node ID are in `firmware/pico/config.py`.
- Set a unique `NODE_ID` per node before flashing.
- `firmware/pico/config.py` is the wiring source of truth; `docs/hardware.md` can drift
  (the pin table there may not match the current config).

## Multi-board deployment

- Nodes share one channel/address; only `NODE_ID` should differ per board.
- With multiple boards attached, prefix commands with a selected port, e.g.
  `mpremote connect /dev/ttyACM0 fs cp firmware/pico/config.py :`; use
  `mpremote devs` to find ports.

- Mesh uses one shared radio address/channel; all nodes must use the same settings.
- `main.py` reads raw USB serial stdin, so the REPL is unavailable until Ctrl-C.
- nRF24 IRQ is intentionally unused in Phase 1 (GP21 reserved for Phase 2); RX is polled.
- `main.py` dedupes with a set and clears all keys after 64 (not an LRU).
- `passwd.txt` is gitignored and contains a secret. Never commit it or any other secret.
