
#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

char *PrSq(const int sq) {

    static char SqStr[3];

    int file = FilesBrd[sq];
    int rank = RanksBrd[sq];

    sprintf(SqStr, "%c%c", ('a' + file), ('1' + rank));

    return SqStr;
}

char *PrMove(const int move) {

    static char MvStr[6];

    int ff = FilesBrd[FROMSQ(move)];
    int rf = RanksBrd[FROMSQ(move)];
    int ft = FilesBrd[TOSQ(move)];
    int rt = RanksBrd[TOSQ(move)];

    int promoted = PROMOTED(move);

    if (promoted) {
        char pchar = 'q';
        if (IsKn(promoted)) {
            pchar = 'n';
        } else if (IsRQ(promoted) && !IsBQ(promoted)) {
            pchar = 'r';
        } else if (!IsRQ(promoted) && IsBQ(promoted)) {
            pchar = 'b';
        }
        sprintf(MvStr, "%c%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt), pchar);
    } else {
        sprintf(MvStr, "%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt));
    }

    return MvStr;
}

int ParseMove(char *ptrChar, S_BOARD *pos) {

    assert(CheckBoard(pos));

    if (ptrChar[1] > '8' || ptrChar[1] < '1')
        return NOMOVE;
    if (ptrChar[3] > '8' || ptrChar[3] < '1')
        return NOMOVE;
    if (ptrChar[0] > 'h' || ptrChar[0] < 'a')
        return NOMOVE;
    if (ptrChar[2] > 'h' || ptrChar[2] < 'a')
        return NOMOVE;

    int from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
    int to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');

    assert(SqOnBoard(from) && SqOnBoard(to));

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);
    int MoveNum = 0;
    int Move = 0;
    int PromPce = EMPTY;

    for (MoveNum = 0; MoveNum < list->count; ++MoveNum) {
        Move = list->moves[MoveNum].move;
        if (FROMSQ(Move) == from && TOSQ(Move) == to) {
            PromPce = PROMOTED(Move);
            if (PromPce != EMPTY) {
                if (IsRQ(PromPce) && !IsBQ(PromPce) && ptrChar[4] == 'r') {
                    return Move;
                } else if (!IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4] == 'b') {
                    return Move;
                } else if (IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4] == 'q') {
                    return Move;
                } else if (IsKn(PromPce) && ptrChar[4] == 'n') {
                    return Move;
                }
                continue;
            }
            return Move;
        }
    }

    return NOMOVE;
}

// kingside == true  -> O-O
// kingside == false -> O-O-O
static int MatchCastle(S_BOARD *pos, int kingside) {

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    int index = 0;
    int move = 0;
    int wantFile = kingside ? FILE_G : FILE_C;

    for (index = 0; index < list->count; ++index) {
        move = list->moves[index].move;

        if (!(move & MFLAGCA))
            continue;
        if (FilesBrd[TOSQ(move)] != wantFile)
            continue;

        if (MakeMove(pos, move)) {
            TakeMove(pos);
            return move;
        }
    }

    return NOMOVE;
}

// Standard Algebraic Notation
// https://www.chessprogramming.org/Algebraic_Chess_Notation#Standard_Algebraic_Notation_.28SAN.29
int ParseSan(char *ptrChar, S_BOARD *pos) {

    assert(CheckBoard(pos));

    char buf[16];
    int n = 0;
    int i = 0;
    for (i = 0; ptrChar[i] && ptrChar[i] != '\n' && ptrChar[i] != ' ' && n < 15; ++i) {
        char c = ptrChar[i];
        // ignore decorations - check(+), mate(#), annotations(!/?)
        if (c == '+' || c == '#' || c == '!' || c == '?')
            continue;
        buf[n++] = c;
    }
    buf[n] = '\0';
    if (n == 0)
        return NOMOVE;

    // castling
    if (strcmp(buf, "O-O-O") == 0 || strcmp(buf, "0-0-0") == 0)
        return MatchCastle(pos, false);
    if (strcmp(buf, "O-O") == 0 || strcmp(buf, "0-0") == 0)
        return MatchCastle(pos, true);

    // coordinate notation
    if (n >= 4 && buf[0] >= 'a' && buf[0] <= 'h' && buf[1] >= '1' && buf[1] <= '8' && buf[2] >= 'a' && buf[2] <= 'h' && buf[3] >= '1' && buf[3] <= '8')
        return ParseMove(buf, pos);

    // SAN
    int pieceType = wP;
    int idx = 0;
    switch (buf[0]) {
    case 'N':
        pieceType = wN;
        idx = 1;
        break;
    case 'B':
        pieceType = wB;
        idx = 1;
        break;
    case 'R':
        pieceType = wR;
        idx = 1;
        break;
    case 'Q':
        pieceType = wQ;
        idx = 1;
        break;
    case 'K':
        pieceType = wK;
        idx = 1;
        break;
    default:
        pieceType = wP;
        idx = 0;
        break;
    }
    int wantPce = (pos->side == WHITE) ? pieceType : pieceType + 6;

    // [disambiguation][destination][promo]
    char t[8];
    int tn = 0;
    for (i = idx; buf[i] && tn < 7; ++i) {
        if (buf[i] == 'x' || buf[i] == '=' || buf[i] == '.')
            continue;
        t[tn++] = buf[i];
    }
    t[tn] = '\0';

    // =promotion
    int wantProm = EMPTY;
    if (tn >= 1) {
        int prom = EMPTY;
        switch (t[tn - 1]) {
        case 'N':
            prom = wN;
            break;
        case 'B':
            prom = wB;
            break;
        case 'R':
            prom = wR;
            break;
        case 'Q':
            prom = wQ;
            break;
        default:
            break;
        }
        if (prom != EMPTY) {
            wantProm = (pos->side == WHITE) ? prom : prom + 6;
            tn--; // consume the promotion char
        }
    }

    // destination
    if (tn < 2)
        return NOMOVE;
    int destFile = t[tn - 2] - 'a';
    int destRank = t[tn - 1] - '1';
    if (destFile < FILE_A || destFile > FILE_H || destRank < RANK_1 || destRank > RANK_8)
        return NOMOVE;
    int destSq = FR2SQ(destFile, destRank);

    int disFile = -1;
    int disRank = -1;
    for (i = 0; i < tn - 2; ++i) {
        if (t[i] >= 'a' && t[i] <= 'h')
            disFile = t[i] - 'a';
        else if (t[i] >= '1' && t[i] <= '8')
            disRank = t[i] - '1';
    }

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    int wantQ = (pos->side == WHITE) ? wQ : bQ;
    int found = NOMOVE;
    int index = 0;
    int move = 0;
    int from = 0;
    int movProm = 0;

    for (index = 0; index < list->count; ++index) {
        move = list->moves[index].move;
        from = FROMSQ(move);

        if (pos->pieces[from] != wantPce)
            continue;
        if (TOSQ(move) != destSq)
            continue;
        if (disFile != -1 && FilesBrd[from] != disFile)
            continue;
        if (disRank != -1 && RanksBrd[from] != disRank)
            continue;

        movProm = PROMOTED(move);
        if (wantProm != EMPTY) {
            if (movProm != wantProm)
                continue;
        } else if (movProm != EMPTY) {
            // default queen promotion
            if (movProm != wantQ)
                continue;
        }

        if (!MakeMove(pos, move))
            continue;
        TakeMove(pos);

        if (found != NOMOVE)
            return NOMOVE;
        found = move;
    }

    return found;
}

void PrintMoveList(const S_MOVELIST *list) {
    int index = 0;
    int score = 0;
    int move = 0;
    printf("MoveList:\n");

    for (index = 0; index < list->count; ++index) {

        move = list->moves[index].move;
        score = list->moves[index].score;

        printf("Move:%d > %s (score:%d)\n", index + 1, PrMove(move), score);
    }
    printf("MoveList Total %d Moves:\n\n", list->count);
}
