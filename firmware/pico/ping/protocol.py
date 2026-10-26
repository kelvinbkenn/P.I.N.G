"""Wire protocol for P.I.N.G text messaging.

Pure Python so it runs on both CPython (host tests) and MicroPython (device).
A packet is exactly PAYLOAD_SIZE bytes, matching the nRF24L01+ payload size.
"""

import struct

PROTOCOL_VERSION = 1
PAYLOAD_SIZE = 32
HEADER_FORMAT = "<BBBBHBB"
HEADER_SIZE = struct.calcsize(HEADER_FORMAT)
MAX_TEXT = PAYLOAD_SIZE - HEADER_SIZE

TYPE_TEXT = 1
TYPE_ACK = 2
TYPE_BEACON = 3
TYPE_AUDIO = 4

BROADCAST = 0xFF


def encode(msg_type, src, dst, seq, payload=b"", flags=0):
    if isinstance(payload, str):
        payload = payload.encode("utf-8")
    payload = bytes(payload)
    if len(payload) > MAX_TEXT:
        raise ValueError("payload too long: %d > %d" % (len(payload), MAX_TEXT))
    header = struct.pack(
        HEADER_FORMAT,
        PROTOCOL_VERSION,
        msg_type & 0xFF,
        src & 0xFF,
        dst & 0xFF,
        seq & 0xFFFF,
        flags & 0xFF,
        len(payload),
    )
    return header + payload + b"\x00" * (MAX_TEXT - len(payload))


def decode(data):
    data = bytes(data)
    if len(data) > PAYLOAD_SIZE:
        raise ValueError("packet too long: %d > %d" % (len(data), PAYLOAD_SIZE))
    if len(data) < PAYLOAD_SIZE:
        data += b"\x00" * (PAYLOAD_SIZE - len(data))
    version, msg_type, src, dst, seq, flags, length = struct.unpack(
        HEADER_FORMAT, data[:HEADER_SIZE]
    )
    if version != PROTOCOL_VERSION:
        raise ValueError("unsupported protocol version: %d" % version)
    if length > MAX_TEXT:
        raise ValueError("invalid payload length: %d" % length)
    payload = data[HEADER_SIZE : HEADER_SIZE + length]
    try:
        text = payload.decode("utf-8")
    except Exception:
        text = ""
    return {
        "version": version,
        "type": msg_type,
        "src": src,
        "dst": dst,
        "seq": seq,
        "flags": flags,
        "payload": payload,
        "text": text,
    }


def text_message(src, dst, seq, text, flags=0):
    return encode(TYPE_TEXT, src, dst, seq, text.encode("utf-8"), flags)


def parse_command(line):
    """Parse a console line into (dst, text), or None when empty/invalid."""
    line = line.strip()
    if not line:
        return None
    if line.startswith("@"):
        target, _, rest = line[1:].partition(" ")
        rest = rest.strip()
        if rest:
            try:
                dst = int(target)
            except ValueError:
                return None
            if 0 <= dst <= 0xFF:
                return dst, rest
        return None
    return BROADCAST, line
