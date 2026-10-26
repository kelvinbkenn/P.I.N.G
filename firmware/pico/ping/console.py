"""Line input over the USB serial console."""

import sys

try:
    import select
except ImportError:  # pragma: no cover - MicroPython always provides select
    select = None


class Console:
    def __init__(self, max_line=200):
        self.max_line = max_line
        self._buffer = ""
        self._poll = None
        if select is not None:
            try:
                self._poll = select.poll()
                self._poll.register(sys.stdin, select.POLLIN)
            except Exception:
                self._poll = None

    def _read_char(self):
        if self._poll is not None:
            if not self._poll.poll(0):
                return None
        elif select is not None:
            if not select.select([sys.stdin], [], [], 0)[0]:
                return None
        else:
            return None
        try:
            char = sys.stdin.read(1)
        except Exception:
            return None
        return char or None

    def poll_line(self):
        char = self._read_char()
        while char is not None:
            if char in ("\r", "\n"):
                if self._buffer:
                    line = self._buffer
                    self._buffer = ""
                    return line
            elif char in ("\b", "\x7f"):
                self._buffer = self._buffer[:-1]
            elif 32 <= ord(char) < 127:
                if len(self._buffer) < self.max_line:
                    self._buffer += char
            char = self._read_char()
        return None

    def println(self, text=""):
        print(text)
