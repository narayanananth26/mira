# mira

> Mom, I Resigned Again

A chess engine written in C (VICE-style).

## Build

```sh
make            # optimized build (asserts off)
make debug      # debug build (asserts on)
```

## Run

```sh
./mira
```

At the prompt, pick a mode:

- `uci` — UCI protocol (for GUIs like Arena, cutechess)
- `xboard` — XBoard/WinBoard protocol
- `mira` — interactive console mode

Pass `nobook` to disable the opening book.

## Features

- 120-square mailbox + bitboard hybrid board representation
- Alpha-beta search with PV table, killers, and history heuristics
- Zobrist hashing with a transposition table
- Polyglot opening book support
- Perft testing for move-generation correctness

## License

[MIT](LICENSE) © Ananth Narayan
