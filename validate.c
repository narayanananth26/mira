#include "defs.h"
#include <stdio.h>
#include <string.h>

bool SqOnBoard(const int sq) { return FilesBrd[sq] != OFFBOARD; }

bool SideValid(const int side) { return (side == WHITE || side == BLACK); }

bool FileRankValid(const int fr) { return (fr >= 0 && fr <= 7); }

bool PieceValidEmpty(const int pce) { return (pce >= EMPTY && pce <= bK); }

bool PieceValid(const int pce) { return (pce >= wP && pce <= bK); }

void MirrorEvalTest(S_BOARD *pos) {
    FILE *file;
    file = fopen("mirror.epd", "r");
    char lineIn[1024];
    int ev1 = 0;
    int ev2 = 0;
    int positions = 0;
    if (file == NULL) {
        printf("File Not Found\n");
        return;
    } else {
        while (fgets(lineIn, 1024, file) != NULL) {
            if (lineIn[0] == '\n' || lineIn[0] == '\0') {
                continue;
            }
            ParseFen(lineIn, pos);
            positions++;
            ev1 = EvaluatePosition(pos);
            MirrorBoard(pos);
            ev2 = EvaluatePosition(pos);

            if (ev1 != ev2) {
                printf("\n\n\n");
                ParseFen(lineIn, pos);
                PrintBoard(pos);
                MirrorBoard(pos);
                PrintBoard(pos);
                printf("\n\nMirror Fail:\n%s\n", lineIn);
                getchar();
                return;
            }

            if ((positions % 1000) == 0) {
                printf("position %d\n", positions);
            }

            memset(&lineIn[0], 0, sizeof(lineIn));
        }
        printf("Mirror test passed: %d positions\n", positions);
        fclose(file);
    }
}
