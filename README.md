# mira

> Mom, I Resigned Again

A chess engine written in C (VICE-style).

## Install

```sh
brew tap narayanananth26/mira
brew trust narayanananth26/mira
brew install mira
```

## Run

```sh
mira            # console mode — play in the terminal (default)
mira uci        # UCI protocol (for GUIs like Arena, cutechess)
mira xboard     # XBoard/WinBoard protocol
```

Pass `nobook` to disable the opening book, e.g. `mira uci nobook`.

## Build from source

```sh
make            # optimized build (asserts off)
make debug      # debug build (asserts on)
./mira          # run the local build
```

## Features

- 120-square mailbox + bitboard hybrid board representation
- Alpha-beta search with PV table, killers, and history heuristics
- Zobrist hashing with a transposition table
- Polyglot opening book support
- Perft testing for move-generation correctness

## License

[MIT](LICENSE) © Ananth Narayan
