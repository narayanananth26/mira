## Enums

### Pieces
`EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK` — 0..12. Indexes into per-piece arrays (`PieceVal`, `PieceCol`, `pceNum`, `pList`, `PieceKeys`, etc.).

### Files / Ranks
`FILE_A..FILE_H, FILE_NONE` and `RANK_1..RANK_8, RANK_NONE` — 0..8.

### Sides
`WHITE, BLACK, BOTH` — 0..2. `BOTH` is used as the third slot for combined-color data (e.g. `pawns[BOTH]`).

### Squares (120-based)
`A1 = 21 .. H8 = 98`, plus `NO_SQ` and `OFFBOARD`. The board uses a 10x12 mailbox layout: 120 squares total, with off-board sentinel ranks/files surrounding the playable 8x8. Conversion to a 0..63 index uses `SQ64(sq120)` / `SQ120(sq64)`.

### Castle Permissions (bitmask)
`WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8` — OR-combined into `castlePerm`.

## Move Encoding (`int move`)

A single 25-bit packed integer:

| Bits | Field | Macro |
|------|-------|-------|
| 0–6 | From square (120-based) | `FROMSQ(m)` |
| 7–13 | To square | `TOSQ(m)` |
| 14–17 | Captured piece | `CAPTURED(m)` |
| 18 | En passant flag | `MFLAGEP` |
| 19 | Pawn-start (double push) flag | `MFLAGPS` |
| 20–23 | Promoted piece | `PROMOTED(m)` |
| 24 | Castle flag | `MFLAGCA` |

Helper masks: `MFLAGCAP` (any capture), `MFLAGPROM` (any promotion). `NOMOVE = 0`.

## Structs

### `S_MOVE`
```c
int move;   // packed move (see above)
int score;  // ordering score for search
```

### `S_MOVELIST`
A fixed-capacity buffer of generated moves for a single position.
```c
S_MOVE moves[MAXPOSITIONMOVES]; // 256
int    count;
```

### `S_UNDO`
Snapshot of state needed to undo a move. Stored in `S_BOARD.history[]`.
```c
int  move;        // the move that was made
int  castlePerm;  // castling rights before the move
int  enPas;       // en-passant target before the move
int  fiftyMove;   // halfmove clock before the move
U64  posKey;      // Zobrist key before the move
```

### `S_BOARD` — the full game state

| Field | Purpose |
|-------|---------|
| `int pieces[BRD_SQ_NUM]` | 120-square mailbox: piece enum on each playable square, `EMPTY` if empty, or `OFFBOARD` (= 100, from the squares enum) on the mailbox border. |
| `U64 pawns[3]` | Pawn bitboards indexed by `WHITE`, `BLACK`, `BOTH`. |
| `int KingSq[2]` | King square (120-based) per color. |
| `int side` | Side to move (`WHITE` / `BLACK`). |
| `int enPas` | En-passant target square, or `NO_SQ`. |
| `int fiftyMove` | Halfmove counter for the 50-move rule. |
| `int ply` | Halfmoves searched from the current root. |
| `int hisPly` | Total halfmoves played in the game (index into `history`). |
| `int castlePerm` | Castling rights bitmask (`WKCA | WQCA | BKCA | BQCA`). |
| `U64 posKey` | Zobrist hash of the current position. |
| `int pceNum[13]` | Count of each piece type currently on the board. |
| `int bigPce[2]` | Non-pawn piece counts per color. |
| `int majPce[2]` | Major piece (rook, queen) counts per color. |
| `int minPce[2]` | Minor piece (knight, bishop) counts per color. |
| `int material[2]` | Total material value per color. |
| `int pList[13][10]` | `pList[pce][i]` = square of the i-th piece of type `pce`. |
| `S_UNDO history[MAXGAMEMOVES]` | Stack of undo records, one per played halfmove. |
