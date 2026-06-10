#include "defs.h"
#include <assert.h>
#include <stdio.h>

#define PERFTFEN "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

int main() {
    AllInit();

    S_BOARD board;

    ParseFen(PERFTFEN, &board);
    PerftTest(3, &board);

    return 0;
}
