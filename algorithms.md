## makemove.c

### ClearPiece(sq, pos)

Removes a piece from a square.

1. Look at what piece is on the square.
2. Figure out its color (white or black).
3. Update the position's hash key to reflect the piece leaving.
4. Empty the square and subtract the piece's value from that side's material total.
5. Update the piece counters:
   - If it's not a pawn: decrease the big-piece count, and either the major or minor count depending on the type.
   - If it's a pawn: clear its bit in that color's pawn bitboard and in the combined pawn bitboard.
6. Find the piece's entry in the piece list.
7. Remove it by replacing it with the last entry in the list and shrinking the list by one.

### AddPiece(sq, pos, pce)

Places a piece on a square.

1. Figure out the piece's color.
2. Update the position's hash key to reflect the new piece.
3. Put the piece on the square.
4. Update the piece counters:
   - If it's not a pawn: increase the big-piece count, and either the major or minor count.
   - If it's a pawn: set its bit in that color's pawn bitboard and in the combined pawn bitboard.
5. Add the piece's value to that side's material total.
6. Append the square to the piece list and increase the count.

### MovePiece(from, to, pos)

Slides a piece from one square to another. The piece counts and material stay the same.

1. Look at the piece on the `from` square and its color.
2. Update the hash key: piece leaves `from`, piece arrives at `to`.
3. Empty the `from` square and put the piece on `to`.
4. If it's a pawn, update the pawn bitboards: clear the bit at `from`, set the bit at `to`.
5. Find the entry for `from` in the piece list and replace it with `to`.

### MakeMove(pos, move)

Plays a move on the board. Saves enough info to undo it later. Returns `false` if the move is illegal (leaves your own king in check).

1. Read the start square, end square, and which side is moving.
2. Save the current position hash to the history stack.
3. Handle special moves first:
   - **En passant**: remove the captured pawn (it's not on the destination square).
   - **Castling**: also slide the rook to the other side of the king.
4. Save the rest of the undo info: the move itself, fifty-move counter, en passant square, and castling rights.
5. Update castling rights using the `CastlePerm` table — moving from or to a king or rook home square loses the matching right.
6. Clear the en passant square.
7. **If a piece is captured** (normal capture, not en passant): remove it from the destination, and reset the fifty-move counter.
8. Advance the move counters.
9. **If a pawn moved**: reset the fifty-move counter. If it was a two-square push, set the new en passant square behind the pawn.
10. Move the piece from start to destination.
11. **If it's a promotion**: remove the pawn that just arrived and add the promoted piece in its place.
12. If a king moved, update its tracked square.
13. Switch sides.
14. **Legality check**: if the king of the side that just moved is now under attack, the move was illegal — return `false`. Otherwise return `true`.

### TakeMove(pos)

Undoes the last move. Mirrors `MakeMove` in reverse.

1. Step the move counters back.
2. Read the saved undo info from history.
3. Restore the en passant square, castling rights, and fifty-move counter.
4. Switch sides back.
5. Reverse special moves:
   - **En passant**: put the captured pawn back on its original square.
   - **Castling**: slide the rook back to its corner.
6. Move the piece from destination back to start.
7. If a king moved, restore its tracked square.
8. **If a piece was captured**: put it back on the destination square.
9. **If it was a promotion**: remove the promoted piece and put a pawn back in its place.
10. Sanity check: the position hash should now match the one we saved before the move.

## movegen.c

Generates the list of pseudo-legal moves for the side to move. "Pseudo-legal" means we don't filter out moves that leave our own king in check here — that's `MakeMove`'s job (it returns `false` for illegal moves).

Each move is packed into a single 25-bit integer via the `MOVE` macro: from-square (bits 0–6), to-square (7–13), captured piece (14–19), promoted piece (20–23), and flag bits (en passant, pawn-start, castling) above that.

### Add helpers

A small family of helpers append moves to the list. `AddQuietMove`, `AddCaptureMove`, and `AddEnPassantMove` just write the move and a zero score into the next slot and bump the count — the score field is filled in later by move ordering.

The pawn helpers wrap the basics with promotion handling:

- `AddWhitePawnMove` / `AddBlackPawnMove`: if the pawn is on the 7th (white) or 2nd (black) rank, emit four separate moves — one promoting to Q, R, B, N. Otherwise emit a single non-promotion move.
- `AddWhitePawnCapMove` / `AddBlackPawnCapMove`: same idea for capture moves, recording the captured piece in each.

### GenerateAllMoves(pos, list)

Fills `list` with every pseudo-legal move for `pos->side`.

1. Reset the list count to zero.
2. **Pawn moves** (split by side because pawns only move forward):
   - For each pawn, try the one-square push if the square ahead is empty. If the pawn is on its starting rank and the square two ahead is also empty, emit a two-square push with the `MFLAGPS` (pawn-start) flag.
   - Try the two diagonal captures: each must be on-board and hold an enemy piece.
   - If `pos->enPas` is set and matches one of the diagonals, emit an en passant move with the `MFLAGEP` flag.
3. **Castling**: for each castling right still held, check the squares between king and rook are empty, and that the king's start square and the square it passes through are not attacked by the enemy. The destination square is checked later by `MakeMove`'s legality test. Emit with the `MFLAGCA` flag.
4. **Sliding pieces** (bishop, rook, queen): use `LoopSlidePce` / `LoopSlideIndex` to iterate the side's sliders. For each piece, walk each direction in `PceDir` one square at a time:
   - If we hit an enemy piece, emit a capture and stop.
   - If we hit our own piece or run off the board, stop.
   - Otherwise emit a quiet move and keep walking.
5. **Non-sliding pieces** (knight, king): same idea but only one step per direction — no inner walk loop. Skip off-board targets, capture enemies, ignore friendlies, and emit quiet moves on empty squares.
