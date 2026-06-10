#include "defs.h"
#include <assert.h>
#include <stdio.h>

#define PERFTFEN "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1"

int main() {
    AllInit();

    S_BOARD board;

    ParseFen(PERFTFEN, &board);

    char input[6];
    int Move = NOMOVE;
    while (true) {
        PrintBoard(&board);
        printf("Enter a move: ");
        fgets(input, 6, stdin);

        if (input[0] == 'q') { // quit
            break;
        } else if (input[0] == 'u') { // undo
            TakeMove(&board);
        } else {
            Move = ParseMove(input, &board);
            if (Move != NOMOVE) {
                MakeMove(&board, Move);
            }
        }

        fflush(stdin);
    }

    return 0;
}
