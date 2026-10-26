# P.I.N.G — agent notes

This is a hardware/decentralized-networking project (Raspberry Pi Pico + nRF24l01+ radio) described in README.md. It is NOT a conventional software repository: there is no package manager, build tool, test suite, lint config, or CI.

- No `npm install` / `pip install` / `make` / test command exists. Do not invent one.
- `passwd.txt` is on disk but gitignored (it contains a secret). Never commit it or any other secret.
- Single branch `main`, tracking `origin/main`; working tree is clean.
- Before adding software, check README.md and confirm the intended entrypoint first.
