#include "defs.h"
#include <assert.h>
#include <stdio.h>

#define PERFTFEN "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1"
// Mate in 3 for White: 1.Qg6! ... 2.Qxg7#/Qxh6+ leading to mate
#define MATE3FEN "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0 1"
// https://www.chessprogramming.org/Perft_Results
#define KIWIPETE "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define ORDERINGFEN "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"

int main() {
    AllInit();

    S_BOARD board[1];
    InitPvTable(board->PvTable);

    S_SEARCHINFO info[1];

    ParseFen(ORDERINGFEN, board);

    char input[6];
    int Move = NOMOVE;
    while (true) {
        PrintBoard(board);
        printf("Enter a move: ");
        fgets(input, 6, stdin);

        if (input[0] == 'q') { // quit
            break;
        } else if (input[0] == 'u') { // undo
            TakeMove(board);
        } else if (input[0] == 't') { // test
            PerftTest(4, board);
        } else if (input[0] == 's') {
            info->depth = 5;
            info->starttime = GetTimeMs();
            info->stoptime = GetTimeMs() + 200000;
            SearchPosition(board, info);
        } else {
            Move = ParseMove(input, board);
            if (Move != NOMOVE) {
                MakeMove(board, Move);
            } else {
                printf("No move parsed: %s\n", input);
            }
        }

        fflush(stdin);
    }

    free(board->PvTable->pTable);

    return 0;
}
