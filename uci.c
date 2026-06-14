#include "defs.h"
#include <stdio.h>
#include <string.h>

#define INPUTBUFFER 400 * 6

void ParseGo(char *line, S_SEARCHINFO *info, S_BOARD *pos) {}

// position fen fenstr
// position startpos
// ... moves e2e4 e7e5 b7b8q
void ParsePosition(char *lineIn, S_BOARD *pos) {

    lineIn += 9;
    char *ptrChar = lineIn;

    if (strncmp(lineIn, "startpos", 8) == 0) {
        ParseFen(START_FEN, pos);
    } else {
        ptrChar = strstr(lineIn, "fen");
        if (ptrChar == NULL) {
            ParseFen(START_FEN, pos);
        } else {
            ptrChar += 4;
            ParseFen(ptrChar, pos);
        }
    }

    ptrChar = strstr(lineIn, "moves");
    int move;

    if (ptrChar != NULL) {
        ptrChar += 6;
        while (*ptrChar) {
            move = ParseMove(ptrChar, pos);
            if (move == NOMOVE)
                break;
            MakeMove(pos, move);
            pos->ply = 0;
            while (*ptrChar && *ptrChar != ' ')
                ptrChar++;
            ptrChar++;
        }
    }
    PrintBoard(pos);
}

void UciLoop() {

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    char line[INPUTBUFFER];
    printf("id name %s\n", NAME);
    printf("id author ananth\n");
    printf("uciok\n");

    S_BOARD pos[1];
    S_SEARCHINFO info[1];
    InitPvTable(pos->PvTable);

    while (true) {
        memset(&line[0], 0, sizeof(line));
        fflush(stdout);
        if (!fgets(line, INPUTBUFFER, stdin))
            continue;

        if (line[0] == '\n')
            continue;

        if (!strncmp(line, "isready", 7)) {
            printf("readyok\n");
            continue;
        } else if (!strncmp(line, "position", 8)) {
            ParsePosition(line, pos);
        } else if (!strncmp(line, "ucinewgame", 10)) {
            ParsePosition("position startpos\n", pos);
        } else if (!strncmp(line, "go", 2)) {
            printf("Seen Go..\n");
            ParseGo(line, info, pos);
        } else if (!strncmp(line, "quit", 4)) {
            info->quit = true;
            break;
        } else if (!strncmp(line, "uci", 3)) {
            printf("id name %s\n", NAME);
            printf("id author ananth\n");
            printf("uciok\n");
        }
        if (info->quit)
            break;
    }

    free(pos->PvTable->pTable);
}
