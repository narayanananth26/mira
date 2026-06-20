#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define PERFTFEN "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1"
// Mate in 3 for White: 1.Qg6! ... 2.Qxg7#/Qxh6+ leading to mate
#define MATE3FEN "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0 1"
// https://www.chessprogramming.org/PerftResults
#define KIWIPETE "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define ORDERINGFEN "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"

int main() {

    AllInit();

    S_BOARD pos[1];
    S_SEARCHINFO info[1];
    InitHashTable(pos->HashTable, 64);

    printf("Welcome to Mira! Type 'mira' for console mode...\n");

    char line[256];
    while (true) {
        memset(&line[0], 0, sizeof(line));

        fflush(stdout);
        if (!fgets(line, 256, stdin))
            continue;
        if (line[0] == '\n')
            continue;
        if (!strncmp(line, "uci", 3)) {
            UciLoop(pos, info);
            if (info->quit == true)
                break;
            continue;
        } else if (!strncmp(line, "xboard", 6)) {
            XBoardLoop(pos, info);
            if (info->quit == true)
                break;
            continue;
        } else if (!strncmp(line, "mira", 4)) {
            ConsoleLoop(pos, info);
            if (info->quit == true)
                break;
            continue;
        } else if ((!strncmp(line, "quit", 4)) || (!strncmp(line, "exit", 4)) || (!strncmp(line, "q", 1))) {
            break;
        }
    }

    free(pos->HashTable->pTable);

    return 0;
}
