#include "defs.h"
#include <stdio.h>
#include <string.h>

void PrintBoardFor(const S_BOARD *pos, const int humanSide) {

    int sq, file, rank, piece;
    int rStart = RANK_8, rEnd = RANK_1, rStep = -1;
    int fStart = FILE_A, fEnd = FILE_H, fStep = 1;

    if (humanSide == BLACK) {
        rStart = RANK_1;
        rEnd = RANK_8;
        rStep = 1;

        fStart = FILE_H;
        fEnd = FILE_A;
        fStep = -1;
    }

    printf("\n");
    for (rank = rStart; rank != rEnd + rStep; rank += rStep) {
        printf("%d", rank + 1);
        for (file = fStart; file != fEnd + fStep; file += fStep) {
            sq = FR2SQ(file, rank);
            piece = pos->pieces[sq];
            printf("%3c", PceChar[piece]);
        }
        printf("\n");
    }

    printf("\n ");
    for (file = fStart; file != fEnd + fStep; file += fStep) {
        printf("%3c", 'a' + file);
    }
    printf("\n\n");
}
