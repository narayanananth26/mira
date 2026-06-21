#include "defs.h"
#include "polykeys.h"
#include <assert.h>

// https://web.archive.org/web/20260603181146/http://hgm.nubati.net/book_format.html
// { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
const int PolyKindOfPiece[13] = {-1, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10};

int HasPawnForCapture(const S_BOARD *board) {
    int sqWithPawn = 0;
    int targetPce = (board->side == WHITE) ? wP : bP;
    if (board->enPas != NO_SQ) {
        if (board->side == WHITE) {
            sqWithPawn = board->enPas - 10;
        } else {
            sqWithPawn = board->enPas + 10;
        }

        if (board->pieces[sqWithPawn + 1] == targetPce) {
            return true;
        } else if (board->pieces[sqWithPawn - 1] == targetPce) {
            return true;
        }
    }
    return false;
}

U64 PolyKeyFromBoard(const S_BOARD *board) {

    int sq = 0, rank = 0, file = 0;
    U64 finalKey = 0;
    int piece = EMPTY;
    int polyPiece = 0;
    int offset = 0;

    for (sq = 0; sq < BRD_SQ_NUM; ++sq) {
        piece = board->pieces[sq];
        if (piece != NO_SQ && piece != EMPTY && piece != OFFBOARD) {
            assert(piece >= wP && piece <= bK);
            polyPiece = PolyKindOfPiece[piece];
            rank = RanksBrd[sq];
            file = FilesBrd[sq];
            finalKey ^= Random64Poly[(64 * polyPiece) + (8 * rank) + file];
        }
    }

    // castling
    offset = 768;

    if (board->castlePerm & WKCA)
        finalKey ^= Random64Poly[offset + 0];
    if (board->castlePerm & WQCA)
        finalKey ^= Random64Poly[offset + 1];
    if (board->castlePerm & BKCA)
        finalKey ^= Random64Poly[offset + 2];
    if (board->castlePerm & BQCA)
        finalKey ^= Random64Poly[offset + 3];

    // enpassant
    offset = 772;
    if (HasPawnForCapture(board) == true) {
        file = FilesBrd[board->enPas];
        finalKey ^= Random64Poly[offset + file];
    }

    if (board->side == WHITE) {
        finalKey ^= Random64Poly[780];
    }
    return finalKey;
}
