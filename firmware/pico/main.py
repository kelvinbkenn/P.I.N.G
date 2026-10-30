"""P.I.N.G Phase 1 entry point: text over nRF24, shown on an OLED."""

import time

import config
from ping import protocol
from ping.buzzer import Buzzer
from ping.console import Console
from ping.display import Display
from ping.radio import Radio

LOOP_DELAY_MS = 5
DEDUPE_LIMIT = 64


def main():
    console = Console()
    display = Display()
    radio = Radio()
    buzzer = Buzzer()

    display.show_status("P.I.N.G", "node %d ready" % config.NODE_ID)
    console.println(
        "P.I.N.G node %d ready. Type text to broadcast, @<id> text to direct." % config.NODE_ID
    )

    seen = set()
    while True:
        line = console.poll_line()
        if line is not None:
            parsed = protocol.parse_command(line)
            if parsed is None:
                console.println("[err] use: text  or  @<id> text")
            else:
                dst, text = parsed
                seq = radio.next_seq()
                packet = protocol.text_message(config.NODE_ID, dst, seq, text)
                ok = radio.send(packet)
                if ok:
                    buzzer.beep(1)
                target = "all" if dst == protocol.BROADCAST else str(dst)
                suffix = "" if ok else " (no ack)"
                console.println("[tx] -> %s: %s%s" % (target, text, suffix))
                display.add_message("tx>", text)

        raw = radio.poll()
        if raw is not None:
            try:
                msg = protocol.decode(raw)
            except ValueError as exc:
                console.println("[rx] bad packet: %s" % exc)
            else:
                if msg["type"] == protocol.TYPE_TEXT and msg["dst"] in (
                    config.NODE_ID,
                    protocol.BROADCAST,
                ):
                    key = (msg["src"], msg["seq"])
                    if key not in seen:
                        seen.add(key)
                        if len(seen) > DEDUPE_LIMIT:
                            seen.clear()
                        label = "@%d" % msg["src"]
                        display.add_message(label, msg["text"])
                        console.println("[rx] %s %s" % (label, msg["text"]))
                        buzzer.beep(2)

        time.sleep_ms(LOOP_DELAY_MS)


main()
