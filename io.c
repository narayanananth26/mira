
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

static char SanLetter(int piece) {
    switch (piece) {
    case wN:
    case bN:
        return 'N';
    case wB:
    case bB:
        return 'B';
    case wR:
    case bR:
        return 'R';
    case wQ:
    case bQ:
        return 'Q';
    case wK:
    case bK:
        return 'K';
    default:
        return 0;
    }
}

char *PrMoveSan(S_BOARD *pos, const int move) {

    static char san[16];

    int from = FROMSQ(move);
    int to = TOSQ(move);
    int piece = pos->pieces[from];
    int promoted = PROMOTED(move);
    int isCap = (CAPTURED(move) != EMPTY) || (move & MFLAGEP);
    char letter = SanLetter(piece);
    int n = 0;
    int index = 0;

    if (move & MFLAGCA) {
        if (FilesBrd[to] == FILE_G) {
            san[n++] = 'O';
            san[n++] = '-';
            san[n++] = 'O';
        } else {
            san[n++] = 'O';
            san[n++] = '-';
            san[n++] = 'O';
            san[n++] = '-';
            san[n++] = 'O';
        }
    } else if (letter) {
        san[n++] = letter;

        S_MOVELIST list[1];
        GenerateAllMoves(pos, list);
        int sameFile = 0;
        int sameRank = 0;
        int ambig = 0;

        for (index = 0; index < list->count; ++index) {
            int m2 = list->moves[index].move;
            if (m2 == move || pos->pieces[FROMSQ(m2)] != piece || TOSQ(m2) != to)
                continue;
            if (!MakeMove(pos, m2))
                continue;
            TakeMove(pos);
            ambig = 1;
            if (FilesBrd[FROMSQ(m2)] == FilesBrd[from])
                sameFile = 1;
            if (RanksBrd[FROMSQ(m2)] == RanksBrd[from])
                sameRank = 1;
        }

        if (ambig) {
            if (!sameFile) {
                san[n++] = 'a' + FilesBrd[from];
            } else if (!sameRank) {
                san[n++] = '1' + RanksBrd[from];
            } else {
                san[n++] = 'a' + FilesBrd[from];
                san[n++] = '1' + RanksBrd[from];
            }
        }

        if (isCap)
            san[n++] = 'x';
        san[n++] = 'a' + FilesBrd[to];
        san[n++] = '1' + RanksBrd[to];
    } else {
        // pawn
        if (isCap) {
            san[n++] = 'a' + FilesBrd[from];
            san[n++] = 'x';
        }
        san[n++] = 'a' + FilesBrd[to];
        san[n++] = '1' + RanksBrd[to];

        if (promoted) {
            san[n++] = '=';
            if (IsKn(promoted))
                san[n++] = 'N';
            else if (IsRQ(promoted) && !IsBQ(promoted))
                san[n++] = 'R';
            else if (IsBQ(promoted) && !IsRQ(promoted))
                san[n++] = 'B';
            else
                san[n++] = 'Q';
        }
    }

    // +/# suffix
    if (MakeMove(pos, move)) {
        if (SqAttacked(pos->KingSq[pos->side], pos->side ^ 1, pos)) {
            S_MOVELIST list[1];
            GenerateAllMoves(pos, list);
            int hasReply = 0;
            for (index = 0; index < list->count; ++index) {
                if (MakeMove(pos, list->moves[index].move)) {
                    TakeMove(pos);
                    hasReply = 1;
                    break;
                }
            }
            san[n++] = hasReply ? '+' : '#';
        }
        TakeMove(pos);
    }

    san[n] = '\0';
    return san;
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

// clang-format off
#define FAIL(r) do { if (reason) *reason = (r); return NOMOVE; } while (0)
// clang-format on

// Standard Algebraic Notation
// https://www.chessprogramming.org/Algebraic_Chess_Notation#Standard_Algebraic_Notation_.28SAN.29
int ParseSan(char *ptrChar, S_BOARD *pos, int *reason) {

    assert(CheckBoard(pos));
    if (reason) {
        *reason = SAN_OK;
    }

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
        FAIL(SAN_MALFORMED);

    // castling
    if (strcmp(buf, "O-O-O") == 0 || strcmp(buf, "0-0-0") == 0) {
        int mv = MatchCastle(pos, false);
        if (mv == NOMOVE && reason) {
            *reason = SAN_ILLEGAL;
        }
        return mv;
    }
    if (strcmp(buf, "O-O") == 0 || strcmp(buf, "0-0") == 0) {
        int mv = MatchCastle(pos, true);
        if (mv == NOMOVE && reason) {
            *reason = SAN_ILLEGAL;
        }
        return mv;
    }

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
        FAIL(SAN_MALFORMED);
    int destFile = t[tn - 2] - 'a';
    int destRank = t[tn - 1] - '1';
    if (destFile < FILE_A || destFile > FILE_H || destRank < RANK_1 || destRank > RANK_8)
        FAIL(SAN_MALFORMED);
    int destSq = FR2SQ(destFile, destRank);

    int disFile = -1;
    int disRank = -1;
    for (i = 0; i < tn - 2; ++i) {
        if (t[i] >= 'a' && t[i] <= 'h')
            disFile = t[i] - 'a';
        else if (t[i] >= '1' && t[i] <= '8')
            disRank = t[i] - '1';
        else
            FAIL(SAN_MALFORMED);
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
            FAIL(SAN_AMBIGUOUS);
        found = move;
    }

    if (found == NOMOVE && reason) {
        *reason = SAN_ILLEGAL;
    }
    return found;
}

#undef FAIL

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
