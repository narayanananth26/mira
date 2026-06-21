#include "defs.h"
#include <_stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define PERFTFEN "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1"
// Mate in 3 for White: 1.Qg6! ... 2.Qxg7#/Qxh6+ leading to mate
#define MATE3FEN "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0 1"
// https://www.chessprogramming.org/PerftResults
#define KIWIPETE "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define ORDERINGFEN "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"

int main(int argc, char *argv[]) {
    AllInit();

    S_BOARD pos[1];
    S_SEARCHINFO info[1];
    info->quit = false;
    pos->HashTable->pTable = NULL;
    InitHashTable(pos->HashTable, 64);
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    // First recognized argument selects the protocol; flags (e.g. nobook) can
    // appear in any position. With no mode given, drop straight into console.
    char *mode = NULL;
    for (int ArgNum = 1; ArgNum < argc; ++ArgNum) {
        if (strcmp(argv[ArgNum], "nobook") == 0) {
            EngineOptions->UseBook = false;
            printf("Book Off\n");
        } else if (strcmp(argv[ArgNum], "uci") == 0 || strcmp(argv[ArgNum], "xboard") == 0 ||
                   strcmp(argv[ArgNum], "console") == 0) {
            mode = argv[ArgNum];
        } else {
            printf("Unknown argument: %s\n", argv[ArgNum]);
            printf("Usage: mira [uci|xboard|console] [nobook]\n");
            free(pos->HashTable->pTable);
            return 1;
        }
    }

    if (mode == NULL || strcmp(mode, "console") == 0) {
        ConsoleLoop(pos, info);
    } else if (strcmp(mode, "uci") == 0) {
        UciLoop(pos, info);
    } else if (strcmp(mode, "xboard") == 0) {
        XBoardLoop(pos, info);
    }

    free(pos->HashTable->pTable);

    return 0;
}
