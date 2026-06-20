#ifndef DEFS_H
#define DEFS_H

#include "stdlib.h"
#include <stdbool.h>

typedef unsigned long long U64;

#define NAME "Mira 1.0"
#define BRD_SQ_NUM 120

#define MAXGAMEMOVES 2048
#define MAXPOSITIONMOVES 256
#define MAXDEPTH 64

// https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
//                 RANK_8/../RANK_1 move castlePerm enPas fiftyMove fullMoves
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define INFINITE 30000

// clang-format off
enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
// clang-format on

enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };

enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

enum { WHITE, BLACK, BOTH };

enum { UCIMODE, XBOARDMODE, CONSOLEMODE };

// clang-format off
enum {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8,
    NO_SQ, OFFBOARD
};
// clang-format on

enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };

typedef struct {
    int move;
    int score;
} S_MOVE;

typedef struct {
    S_MOVE moves[MAXPOSITIONMOVES];
    int count;
} S_MOVELIST;

typedef struct {
    U64 posKey;
    int move;
} S_PVENTRY;

typedef struct {
    S_PVENTRY *pTable;
    int numEntries;
} S_PVTABLE;

typedef struct {

    int move;
    int castlePerm;
    int enPas;
    int fiftyMove;
    U64 posKey;

} S_UNDO;

typedef struct {

    int pieces[BRD_SQ_NUM];
    U64 pawns[3];

    int KingSq[2];

    int side;
    int enPas;
    int fiftyMove;

    int ply;
    int hisPly;

    int castlePerm;

    U64 posKey;

    int pceNum[13]; // no. of pieces of each type on the board
    int bigPce[2];
    int majPce[2];
    int minPce[2];
    int material[2];

    int pList[13][10]; // pList[pieceType][nth piece of pieceType] = sq
    S_PVTABLE PvTable[1];
    int PvArray[MAXDEPTH];

    S_UNDO history[MAXGAMEMOVES];

    // move-ordering heuristics
    int searchHistory[13][BRD_SQ_NUM]; // inc [piece][toSq], when a move beats alpha
    int searchKillers[2][MAXDEPTH];    // store two quiet moves that cross the beta cut off

} S_BOARD;

typedef struct {

    int starttime;
    int stoptime;
    int depth;
    int depthset;
    int timeset;
    int movestogo;
    int infinite;

    long nodes;

    int quit;
    int stopped;

    float fh;  // fail high
    float fhf; // fail high first

    int GAME_MODE;
    int POST_THINKING;

} S_SEARCHINFO;

/* GAME MOVES */
/*
0000 0000 0000 0000 0000 0111 1111 -> From 0x7F
0000 0000 0000 0011 1111 1000 0000 -> To >> 7, 0x7F
0000 0000 0011 1100 0000 0000 0000 -> Captured >> 14, 0xF
0000 0000 0100 0000 0000 0000 0000 -> EP 0x40000
0000 0000 1000 0000 0000 0000 0000 -> Pawn Start 0x80000
0000 1111 0000 0000 0000 0000 0000 -> Promoted Piece >> 20, 0xF
0001 0000 0000 0000 0000 0000 0000 -> Castle 0x1000000
*/

#define FROMSQ(m) ((m) & 0x7F)
#define TOSQ(m) (((m) >> 7) & 0x7F)
#define CAPTURED(m) (((m) >> 14) & 0xF)
#define PROMOTED(m) (((m) >> 20) & 0xF)

#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000

#define MFLAGCAP 0x7C000
#define MFLAGPROM 0xF00000

#define NOMOVE 0

/* MACROS */

#define FR2SQ(f, r) ((21 + (f)) + ((r) * 10))
#define SQ64(sq120) (Sq120ToSq64[(sq120)])
#define SQ120(sq64) (Sq64ToSq120[(sq64)])
#define POP(b) PopBit(b)
#define CNT(b) CountBits(b)
#define CLRBIT(bb, sq) ((bb) &= ClearMask[(sq)])
#define SETBIT(bb, sq) ((bb) |= SetMask[(sq)])

#define IsBQ(p) (PieceBishopQueen[(p)])
#define IsRQ(p) (PieceRookQueen[(p)])
#define IsKn(p) (PieceKnight[(p)])
#define IsKi(p) (PieceKing[(p)])

#define MIRROR64(sq) (Mirror64[(sq)])

/* GLOBALS */

extern int Sq120ToSq64[BRD_SQ_NUM];
extern int Sq64ToSq120[64];
extern U64 SetMask[64];
extern U64 ClearMask[64];

extern U64 PieceKeys[13][120];
extern U64 SideKey;
extern U64 CastleKeys[16];

extern char PceChar[];
extern char SideChar[];
extern char RankChar[];
extern char FileChar[];

extern bool PieceBig[13];
extern bool PieceMaj[13];
extern bool PieceMin[13];
extern int PieceVal[13];
extern int PieceCol[13];
extern bool PiecePawn[13];

extern int FilesBrd[BRD_SQ_NUM];
extern int RanksBrd[BRD_SQ_NUM];

extern bool PieceKnight[13];
extern bool PieceKing[13];
extern bool PieceRookQueen[13];
extern bool PieceBishopQueen[13];
extern bool PieceSlides[13];

extern int Mirror64[64];
/* FUNCTIONS */

// init.c
extern void AllInit();

// bitboards.c
extern void PrintBitBoard(U64 bb);
extern int PopBit(U64 *bb);
extern int CountBits(U64 b);

// hashkeys.c
extern U64 GeneratePosKey(const S_BOARD *pos);

// board.c
extern void ResetBoard(S_BOARD *pos);
extern int ParseFen(char *fen, S_BOARD *pos);
extern void PrintBoard(const S_BOARD *pos);
extern void PrintBoard120(const S_BOARD *pos);
extern void UpdateListsMaterial(S_BOARD *pos);
extern bool CheckBoard(const S_BOARD *pos);
extern void MirrorBoard(S_BOARD *pos);

// attack.c
extern bool SqAttacked(const int sq, const int side, const S_BOARD *pos);

// io.c
extern char *PrSq(const int sq);
extern char *PrMove(const int move);
extern void PrintMoveList(const S_MOVELIST *list);
extern int ParseMove(char *ptrChar, S_BOARD *pos);

extern bool SqOnBoard(const int sq);
extern bool SideValid(const int side);
extern bool FileRankValid(const int fr);
extern bool PieceValidEmpty(const int pce);
extern bool PieceValid(const int pce);

// movegen.c
extern void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list);
extern void GenerateAllCaps(const S_BOARD *pos, S_MOVELIST *list);
extern int MoveExists(S_BOARD *pos, const int move);
extern void InitMvvLva();

// search.c
extern void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info);

// makemove.c
extern void TakeMove(S_BOARD *pos);
extern bool MakeMove(S_BOARD *pos, int move);

// perft.c
extern void PerftTest(int depth, S_BOARD *pos);

// misc.c
extern int GetTimeMs();

// pvtable.c
extern void InitPvTable(S_PVTABLE *table);
extern void StorePvMove(const S_BOARD *pos, const int move);
extern int ProbePvTable(const S_BOARD *pos);
extern void ClearPvTable(S_PVTABLE *table);
extern int GetPvLine(const int depth, S_BOARD *pos);

// evaluate.c
extern int EvaluatePosition(const S_BOARD *pos);

// uci.c
extern void UciLoop(S_BOARD *pos, S_SEARCHINFO *info);

// xboard.c
extern void ConsoleLoop(S_BOARD *pos, S_SEARCHINFO *info);
extern void XBoardLoop(S_BOARD *pos, S_SEARCHINFO *info);

#endif
