#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int ThreeFoldRep(const S_BOARD *pos) {

    assert(CheckBoard(pos));

    int i = 0, r = 0;
    for (i = 0; i < pos->hisPly; ++i) {
        if (pos->history[i].posKey == pos->posKey) {
            r++;
        }
    }
    return r;
}

int DrawMaterial(const S_BOARD *pos) {
    assert(CheckBoard(pos));

    // neither side can mate
    if (pos->pceNum[wP] || pos->pceNum[bP])
        return false;
    if (pos->pceNum[wQ] || pos->pceNum[bQ] || pos->pceNum[wR] || pos->pceNum[bR])
        return false;
    // 2 bishops
    if (pos->pceNum[wB] > 1 || pos->pceNum[bB] > 1) {
        return false;
    }
    // 2 knights
    if (pos->pceNum[wN] > 1 || pos->pceNum[bN] > 1) {
        return false;
    }
    // 1 knight + 1 bishop
    if (pos->pceNum[wN] && pos->pceNum[wB]) {
        return false;
    }
    if (pos->pceNum[bN] && pos->pceNum[bB]) {
        return false;
    }

    return true;
}

int checkresult(S_BOARD *pos) {
    assert(CheckBoard(pos));

    if (pos->fiftyMove > 100) {
        printf("1/2-1/2 {fifty move rule (claimed by mira)}\n");
        return true;
    }

    if (ThreeFoldRep(pos) >= 2) {
        printf("1/2-1/2 {3-fold repetition (claimed by mira)}\n");
        return true;
    }

    if (DrawMaterial(pos) == true) {
        printf("1/2-1/2 {insufficient material (claimed by mira)}\n");
        return true;
    }

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    int MoveNum = 0;
    int found = 0;
    for (MoveNum = 0; MoveNum < list->count; ++MoveNum) {

        if (!MakeMove(pos, list->moves[MoveNum].move)) {
            continue;
        }
        found++;
        TakeMove(pos);
        break;
    }

    if (found != 0)
        return false;

    int InCheck = SqAttacked(pos->KingSq[pos->side], pos->side ^ 1, pos);

    if (InCheck == true) {
        if (pos->side == WHITE) {
            printf("0-1 {black mates (claimed by mira)}\n");
            return true;
        } else {
            printf("0-1 {white mates (claimed by mira)}\n");
            return true;
        }
    } else {
        printf("\n1/2-1/2 {stalemate (claimed by mira)}\n");
        return true;
    }
    return false;
}

void XBoardLoop(S_BOARD *pos, S_SEARCHINFO *info) {

    info->GAME_MODE = XBOARDMODE;
    info->POST_THINKING = true;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    int depth = -1, movestogo[2] = {30, 30}, movetime = -1;
    int time = -1, inc = 0;
    int engineSide = BOTH;
    int timeLeft;
    int sec;
    int mps;
    int move = NOMOVE;
    char inBuf[80], command[80];

    engineSide = BLACK;
    ParseFen(START_FEN, pos);
    depth = -1;
    time = -1;

    while (true) {

        fflush(stdout);

        if (pos->side == engineSide && checkresult(pos) == false) {
            info->starttime = GetTimeMs();
            info->depth = depth;

            if (time != -1) {
                info->timeset = true;
                time /= movestogo[pos->side];
                time -= 50;
                info->stoptime = info->starttime + time + inc;
            }

            if (depth == -1 || depth > MAXDEPTH) {
                info->depth = MAXDEPTH;
            }

            printf("time:%d start:%d stop:%d depth:%d timeset:%d movestogo:%d mps:%d\n", time, info->starttime, info->stoptime, info->depth, info->timeset, movestogo[pos->side], mps);
            SearchPosition(pos, info);

            if (mps != 0) {
                movestogo[pos->side ^ 1]--;
                if (movestogo[pos->side ^ 1] < 1) {
                    movestogo[pos->side ^ 1] = mps;
                }
            }
        }

        fflush(stdout);

        memset(&inBuf[0], 0, sizeof(inBuf));
        fflush(stdout);
        if (!fgets(inBuf, 80, stdin))
            continue;

        sscanf(inBuf, "%s", command);

        printf("command seen:%s\n", inBuf);

        if (!strcmp(command, "quit")) {
            info->quit = true;
            break;
        }

        if (!strcmp(command, "force")) {
            engineSide = BOTH;
            continue;
        }

        if (!strcmp(command, "protover")) {
            printf("feature ping=1 setboard=1 colors=0 usermove=1\n");
            printf("feature done=1\n");
            continue;
        }

        if (!strcmp(command, "sd")) {
            sscanf(inBuf, "sd %d", &depth);
            printf("DEBUG depth:%d\n", depth);
            continue;
        }

        if (!strcmp(command, "st")) {
            sscanf(inBuf, "st %d", &movetime);
            printf("DEBUG movetime:%d\n", movetime);
            continue;
        }

        if (!strcmp(command, "time")) {
            sscanf(inBuf, "time %d", &time);
            time *= 10;
            printf("DEBUG time:%d\n", time);
            continue;
        }

        if (!strcmp(command, "level")) {
            sec = 0;
            movetime = -1;
            if (sscanf(inBuf, "level %d %d %d", &mps, &timeLeft, &inc) != 3) {
                sscanf(inBuf, "level %d %d:%d %d", &mps, &timeLeft, &sec, &inc);
                printf("DEBUG level with :\n");
            } else {
                printf("DEBUG level without :\n");
            }
            timeLeft *= 60000;
            timeLeft += sec * 1000;
            movestogo[0] = movestogo[1] = 30;
            if (mps != 0) {
                movestogo[0] = movestogo[1] = mps;
            }
            time = -1;
            printf("DEBUG level timeLeft:%d movesToGo:%d inc:%d mps%d\n", timeLeft, movestogo[0], inc, mps);
            continue;
        }

        if (!strcmp(command, "ping")) {
            printf("pong%s\n", inBuf + 4);
            continue;
        }

        if (!strcmp(command, "new")) {
            engineSide = BLACK;
            ParseFen(START_FEN, pos);
            depth = -1;
            time = -1;
            continue;
        }

        if (!strcmp(command, "setboard")) {
            engineSide = BOTH;
            ParseFen(inBuf + 9, pos);
            continue;
        }

        if (!strcmp(command, "go")) {
            engineSide = pos->side;
            continue;
        }

        if (!strcmp(command, "usermove")) {
            movestogo[pos->side]--;
            move = ParseMove(inBuf + 9, pos);
            if (move == NOMOVE)
                continue;
            MakeMove(pos, move);
            pos->ply = 0;
        }
    }
}
