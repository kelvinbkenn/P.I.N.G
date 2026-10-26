# P.I.N.G — agent notes

Embedded hardware project: Raspberry Pi Pico 2 (RP2350) + nRF24L01+ radio, MicroPython
firmware, SSD1306 128x64 OLED. Not a conventional app; there is no compile step.

## Commands

- Host tests: `python -m pytest` (pure-Python protocol only, no hardware).
- Format/lint: `ruff format .` then `ruff check .` (config in `pyproject.toml`, line 100).
- Deploy to a board: `mpremote fs cp ...` (see README); run with `mpremote run firmware/pico/main.py`.
- There is no build system, package manifest, or CI. Do not invent `npm`/`pip install`/`make` steps.

## Layout / conventions

- `firmware/pico/ping/` — application modules; `ping/protocol.py` must stay pure Python so host tests can import it.
- `firmware/pico/lib/` — vendored `nrf24l01.py` and `ssd1306.py`; treat as third-party, do not edit.
- `firmware/pico/tests/` — on-device bring-up scripts, not pytest.
- `tests/` — host pytest suite (`conftest.py` adds `firmware/pico` to `sys.path`).
- Pins/channel/address live in `firmware/pico/config.py` (top-level `config` import); set `NODE_ID` per node.

## Gotchas

- `main.py` reads raw USB serial stdin, so the REPL is unavailable until Ctrl-C.
- nRF24 IRQ is **not used in Phase 1** (GP21 reserved for Phase 2); the radio is polled.
- `passwd.txt` is gitignored and contains a secret. Never commit it or any other secret.
- Node-to-node mesh shares one radio address; all nodes must use the same channel/address.
