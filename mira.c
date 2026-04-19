#include "defs.h"
#include "stdio.h"

int main() {
    AllInit();

    int index = 0;
    U64 playBitBoard = 0ULL;

    // for (index = 0; index < 64; index++) {
    //     printf("Index: %d\n", index);
    //     PrintBitBoard(SetMask[index]);
    //     printf("\n");
    // }

    SETBIT(playBitBoard, 61);
    PrintBitBoard(playBitBoard);
    CLRBIT(playBitBoard, 61);
    PrintBitBoard(playBitBoard);

    printf("\n\n");

    return 0;
}
