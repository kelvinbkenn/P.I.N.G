# Roadmap

## Phase 1 — text messaging + OLED

- MicroPython on Raspberry Pi Pico 2.
- Text over nRF24L01+, rendered on a 128x64 SSD1306 OLED.
- Input over USB serial.
- Direct node-to-node mesh (shared channel/address).

## Phase 2 — phone voice calls

- Add a gateway board with BLE or Wi-Fi (Pico 2 W / ESP32 / Raspberry Pi).
- Connect the nRF24 IRQ pin (GP21) and move to interrupt-driven RX.
- Add `AUDIO` packets, fragmentation, a codec (ADPCM/Codec2) and a jitter buffer.
- Half-duplex push-to-talk to start.

## Phase 3 — self-hosted website

- Tiny HTTP server on the gateway board.
- Status page on the LAN; optionally post messages into the mesh.

See `docs/protocol.md` and `docs/hardware.md`.
