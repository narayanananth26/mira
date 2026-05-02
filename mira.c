#include "defs.h"
#include <assert.h>
#include <stdio.h>

#define PAWNMOVES "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"

void ShowSqByAtSide(const int side, const S_BOARD *pos) {

    int rank;
    int file;
    int sq;

    printf("\n\nSquares attacked by: %c\n", SideChar[side]);
    for (rank = RANK_8; rank >= RANK_1; rank--) {
        for (file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file, rank);
            if (SqAttacked(sq, side, pos) == true) {
                printf("X");
            } else
                printf("-");
        }
        printf("\n");
    }

    printf("\n");
}

int main() {
    AllInit();

    S_BOARD board[1];

    ParseFen(PAWNMOVES, board);
    PrintBoard(board);

    S_MOVELIST list[1];

    GenerateAllMoves(board, list);
    PrintMoveList(list);

    return 0;
}
