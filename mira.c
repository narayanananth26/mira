#include "defs.h"
#include "stdio.h"

int main() {
    AllInit();

    U64 playBitBoard = 0ULL;

    printf("Start:\n");
    PrintBitBoard(playBitBoard);

    playBitBoard |= (1ULL << SQ64(D2));
    printf("D2 Added:\n");
    PrintBitBoard(playBitBoard);

    playBitBoard |= (1ULL << SQ64(G2));
    printf("G2 Added:\n");
    PrintBitBoard(playBitBoard);

    playBitBoard |= (1ULL << SQ64(A1));
    printf("A1 Added:\n");
    PrintBitBoard(playBitBoard);

    return 0;
}
