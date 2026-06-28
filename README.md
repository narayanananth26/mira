# mira

> Mom, I Resigned Again

A chess engine written in C ([VICE-style](https://github.com/bluefeversoft/vice/tree/main)).

## Install

```sh
brew tap narayanananth26/mira
brew install mira
```

## Run

```sh
mira            # console mode (default)
mira uci        # UCI protocol
```

Pass `nobook` to disable the opening book - `mira uci nobook`.

## Build from source

```sh
make            # optimized build (asserts off)
make debug      # debug build (asserts on)
./mira          # run the local build
```

## Features

- 120-square mailbox + bitboard hybrid board representation
- Alpha-beta search with PV table, killers, and history heuristics
- Worker thread for search
- Zobrist hashing with a transposition table
- Polyglot opening book support

## To Do

- Parallel search using [Lazy SMP](https://www.chessprogramming.org/Lazy_SMP)
- Perf tuning (PVS/LMR/Aspiration)
- Magic bitboards for big pieces (maybe)

## License

[MIT](LICENSE) © Ananth Narayan
