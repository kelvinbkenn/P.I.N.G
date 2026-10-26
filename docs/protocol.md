# Wire protocol (Phase 1)

Every packet is exactly **32 bytes**, matching the nRF24L01+ payload size.

## Layout

| Offset | Size | Field | Notes |
|---|---|---|---|
| 0 | 1 | version | currently `1` |
| 1 | 1 | type | see below |
| 2 | 1 | src | sender node ID |
| 3 | 1 | dst | recipient node ID, `0xFF` = broadcast |
| 4 | 2 | seq | little-endian sequence number |
| 6 | 1 | flags | reserved |
| 7 | 1 | length | payload length, max `24` |
| 8 | 24 | payload | UTF-8 text, zero padded |

`HEADER_FORMAT = "<BBBBHBB"`, `HEADER_SIZE = 8`, `MAX_TEXT = 24`.

## Types

| Value | Name | Purpose |
|---|---|---|
| 1 | `TEXT` | chat message |
| 2 | `ACK` | reserved |
| 3 | `BEACON` | reserved |
| 4 | `AUDIO` | reserved for Phase 2 |

## Console commands

Plain text is broadcast. Prefix with `@<id>` to target one node.

```
hello world      # broadcast
@2 hello         # send to node 2
```

Received packets are echoed to USB serial and shown on the OLED. Duplicates are
ignored using `(src, seq)`.

## Implementation

`firmware/pico/ping/protocol.py` is pure Python and covered by `tests/test_protocol.py`.
