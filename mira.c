#include "defs.h"
#include "stdio.h"

int main() {
    AllInit();

    U64 playBitBoard = 0ULL;

    playBitBoard |= (1ULL << SQ64(D2));
    playBitBoard |= (1ULL << SQ64(G2));
    playBitBoard |= (1ULL << SQ64(A1));

    printf("\n");
    PrintBitBoard(playBitBoard);

    int count = CNT(playBitBoard);
    printf("Count = %d\n", count);

    int index = POP(&playBitBoard);
    printf("index = %d\n", count);

    PrintBitBoard(playBitBoard);

    count = CNT(playBitBoard);
    printf("Count = %d\n", count);

    printf("\n\n");

    return 0;
}
