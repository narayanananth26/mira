#include "defs.h"
#include <assert.h>
#include <stdio.h>

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
