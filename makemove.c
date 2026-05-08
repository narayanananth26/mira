#include "defs.h"
#include <assert.h>

#define HASH_PCE(pce, sq) (pos->posKey ^= (PieceKeys[(pce)][(sq)]))
#define HASH_CA (pos->posKey ^= (CastleKeys[(pos->castlePerm)]))
#define HASH_SIDE (pos->posKey ^= (SideKey))
#define HASH_EP (pos->posKey ^= (PieceKeys[EMPTY][(pos->enPas)]))

// clang-format off
const int CastlePerm[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

// clang-format on

static void ClearPiece(const int sq, S_BOARD *pos) {

    assert(SqOnBoard(sq));
    assert(CheckBoard(pos));

    int pce = pos->pieces[sq];

    assert(PieceValid(pce));

    int col = PieceCol[pce];
    int index = 0;
    int t_pceNum = -1;

    assert(SideValid(col));

    HASH_PCE(pce, sq);

    pos->pieces[sq] = EMPTY;
    pos->material[col] -= PieceVal[pce];

    if (PieceBig[pce]) {
        pos->bigPce[col]--;
        if (PieceMaj[pce]) {
            pos->majPce[col]--;
        } else {
            pos->minPce[col]--;
        }
    } else {
        CLRBIT(pos->pawns[col], SQ64(sq));
        CLRBIT(pos->pawns[BOTH], SQ64(sq));
    }

    for (index = 0; index < pos->pceNum[pce]; ++index) {
        if (pos->pList[pce][index] == sq) {
            t_pceNum = index;
            break;
        }
    }

    assert(t_pceNum != -1);
    assert(t_pceNum >= 0 && t_pceNum < 10);

    pos->pceNum[pce]--;

    pos->pList[pce][t_pceNum] = pos->pList[pce][pos->pceNum[pce]];
}

static void AddPiece(const int sq, S_BOARD *pos, const int pce) {

    assert(PieceValid(pce));
    assert(SqOnBoard(sq));

    int col = PieceCol[pce];
    assert(SideValid(col));

    HASH_PCE(pce, sq);

    pos->pieces[sq] = pce;

    if (PieceBig[pce]) {
        pos->bigPce[col]++;
        if (PieceMaj[pce]) {
            pos->majPce[col]++;
        } else {
            pos->minPce[col]++;
        }
    } else {
        SETBIT(pos->pawns[col], SQ64(sq));
        SETBIT(pos->pawns[BOTH], SQ64(sq));
    }

    pos->material[col] += PieceVal[pce];
    pos->pList[pce][pos->pceNum[pce]++] = sq;
}

static void MovePiece(const int from, const int to, S_BOARD *pos) {

    assert(SqOnBoard(from));
    assert(SqOnBoard(to));

    int index = 0;
    int pce = pos->pieces[from];
    int col = PieceCol[pce];
    assert(SideValid(col));
    assert(PieceValid(pce));

#ifdef DEBUG
    int t_PieceNum = false;
#endif

    HASH_PCE(pce, from);
    pos->pieces[from] = EMPTY;

    HASH_PCE(pce, to);
    pos->pieces[to] = pce;

    if (!PieceBig[pce]) {
        CLRBIT(pos->pawns[col], SQ64(from));
        CLRBIT(pos->pawns[BOTH], SQ64(from));
        SETBIT(pos->pawns[col], SQ64(to));
        SETBIT(pos->pawns[BOTH], SQ64(to));
    }

    for (index = 0; index < pos->pceNum[pce]; ++index) {
        if (pos->pList[pce][index] == from) {
            pos->pList[pce][index] = to;
#ifdef DEBUG
            t_PieceNum = true;
#endif
            break;
        }
    }
    assert(t_PieceNum);
}
