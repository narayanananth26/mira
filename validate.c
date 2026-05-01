#include "defs.h"

bool SqOnBoard(const int sq) { return FilesBrd[sq] != OFFBOARD; }

bool SideValid(const int side) { return (side == WHITE || side == BLACK); }

bool FileRankValid(const int fr) { return (fr >= 0 && fr <= 7); }

bool PieceValidEmpty(const int pce) { return (pce >= EMPTY && pce <= bK); }

bool PieceValid(const int pce) { return (pce >= wP && pce <= bK); }
