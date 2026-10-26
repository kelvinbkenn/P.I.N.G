import pytest

from ping import protocol


def test_round_trip_text():
    packet = protocol.text_message(1, protocol.BROADCAST, 42, "hello")
    assert len(packet) == protocol.PAYLOAD_SIZE
    msg = protocol.decode(packet)
    assert msg["type"] == protocol.TYPE_TEXT
    assert msg["src"] == 1
    assert msg["dst"] == protocol.BROADCAST
    assert msg["seq"] == 42
    assert msg["text"] == "hello"


def test_max_text_length():
    text = "x" * protocol.MAX_TEXT
    packet = protocol.text_message(2, 1, 1, text)
    assert protocol.decode(packet)["text"] == text


def test_payload_too_long():
    with pytest.raises(ValueError):
        protocol.text_message(1, 2, 1, "x" * (protocol.MAX_TEXT + 1))


def test_decode_pads_short_packet():
    packet = protocol.text_message(1, 2, 7, "hi")
    msg = protocol.decode(packet[: protocol.HEADER_SIZE + 2])
    assert msg["text"] == "hi"
    assert msg["seq"] == 7


def test_decode_rejects_wrong_version():
    packet = bytearray(protocol.text_message(1, 2, 1, "hi"))
    packet[0] = 99
    with pytest.raises(ValueError):
        protocol.decode(packet)


def test_decode_unicode():
    packet = protocol.text_message(1, 2, 1, "hé")
    assert protocol.decode(packet)["text"] == "hé"


def test_parse_broadcast():
    assert protocol.parse_command("hello world") == (
        protocol.BROADCAST,
        "hello world",
    )


def test_parse_directed():
    assert protocol.parse_command("@3 hello") == (3, "hello")


def test_parse_empty():
    assert protocol.parse_command("   ") is None


def test_parse_bad_directive():
    assert protocol.parse_command("@abc hi") is None
    assert protocol.parse_command("@9") is None
