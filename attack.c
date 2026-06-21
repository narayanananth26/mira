#include "defs.h"
#include <assert.h>

const int KnDir[8] = {-8, -19, -21, -12, 8, 19, 21, 12};
const int RkDir[4] = {-1, -10, 1, 10};
const int BiDir[4] = {-9, -11, 11, 9};
const int KiDir[8] = {-1, -10, 1, 10, -9, -11, 11, 9};

bool SqAttacked(const int sq, const int side, const S_BOARD *pos) {

    int pce, index, t_sq, dir;

    assert(SqOnBoard(sq));
    assert(SideValid(side));
    assert(CheckBoard(pos));

    // pawns
    if (side == WHITE) {
        if (pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP) {
            return true;
        }
    } else {
        if (pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP) {
            return true;
        }
    }

    // knights
    for (index = 0; index < 8; ++index) {
        pce = pos->pieces[sq + KnDir[index]];
        assert(PceValidEmptyOffbrd(pce));
        if (pce != OFFBOARD && IsKn(pce) && PieceCol[pce] == side) {
            return true;
        }
    }

    // rooks, queens
    for (index = 0; index < 4; ++index) {
        dir = RkDir[index];
        t_sq = sq + dir;
        assert(SqIs120(t_sq));
        pce = pos->pieces[t_sq];
        assert(PceValidEmptyOffbrd(pce));
        while (pce != OFFBOARD) {
            if (pce != EMPTY) {
                if (IsRQ(pce) && PieceCol[pce] == side) {
                    return true;
                }
                break;
            }
            t_sq += dir;
            assert(SqIs120(t_sq));
            pce = pos->pieces[t_sq];
        }
    }

    // bishops, queens
    for (index = 0; index < 4; ++index) {
        dir = BiDir[index];
        t_sq = sq + dir;
        assert(SqIs120(t_sq));
        pce = pos->pieces[t_sq];
        assert(PceValidEmptyOffbrd(pce));
        while (pce != OFFBOARD) {
            if (pce != EMPTY) {
                if (IsBQ(pce) && PieceCol[pce] == side) {
                    return true;
                }
                break;
            }
            t_sq += dir;
            assert(SqIs120(t_sq));
            pce = pos->pieces[t_sq];
        }
    }

    // kings
    for (index = 0; index < 8; ++index) {
        pce = pos->pieces[sq + KiDir[index]];
        assert(PceValidEmptyOffbrd(pce));
        if (pce != OFFBOARD && IsKi(pce) && PieceCol[pce] == side) {
            return true;
        }
    }

    return false;
}
