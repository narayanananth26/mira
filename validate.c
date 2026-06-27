#include "defs.h"
#include <stdio.h>
#include <string.h>

int MoveListOk(const S_MOVELIST *list, const S_BOARD *pos) {
    if (list->count < 0 || list->count >= MAXPOSITIONMOVES) {
        return false;
    }

    int MoveNum;
    int from = 0;
    int to = 0;
    for (MoveNum = 0; MoveNum < list->count; ++MoveNum) {
        to = TOSQ(list->moves[MoveNum].move);
        from = FROMSQ(list->moves[MoveNum].move);
        if (!SqOnBoard(to) || !SqOnBoard(from)) {
            return false;
        }
        if (!PieceValid(pos->pieces[from])) {
            PrintBoard(pos);
            return false;
        }
    }

    return true;
}

bool SqIs120(const int sq) { return (sq >= 0 && sq < 120); }

bool PceValidEmptyOffbrd(const int pce) { return (PieceValidEmpty(pce) || pce == OFFBOARD); }

bool SqOnBoard(const int sq) { return FilesBrd[sq] != OFFBOARD; }

bool SideValid(const int side) { return (side == WHITE || side == BLACK); }

bool FileRankValid(const int fr) { return (fr >= 0 && fr <= 7); }

bool PieceValidEmpty(const int pce) { return (pce >= EMPTY && pce <= bK); }

bool PieceValid(const int pce) { return (pce >= wP && pce <= bK); }

void DebugAnalysisTest(S_BOARD *pos, S_SEARCHINFO *info, S_HASHTABLE *table) {

    FILE *file;
    file = fopen("lct2.epd", "r");
    char lineIn[1024];

    info->depth = MAXDEPTH;
    info->timeset = true;
    int time = 1140000;

    if (file == NULL) {
        printf("File Not Found\n");
        return;
    } else {
        while (fgets(lineIn, 1024, file) != NULL) {
            info->starttime = GetTimeMs();
            info->stoptime = info->starttime + time;
            ClearHashTable(table);
            ParseFen(lineIn, pos);
            printf("\n%s\n", lineIn);
            printf("time:%d start:%d stop:%d depth:%d timeset:%d\n", time, info->starttime, info->stoptime, info->depth, info->timeset);
            SearchPosition(pos, info, table);
            memset(&lineIn[0], 0, sizeof(lineIn));
        }
    }
}

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
