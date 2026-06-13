#include "defs.h"

char PceChar[] = ".PNBRQKpnbrqk";
char SideChar[] = "wb-";
char RankChar[] = "12345678";
char FileChar[] = "abcdefgh";

// clang-format off
//                       EMPTY  P      N      B      R      Q      K      p      n      b      r      q      k
bool PieceBig[13]      = {false, false, true,  true,  true,  true,  true,  false, true,  true,  true,  true,  true};
bool PieceMaj[13]      = {false, false, false, false, true,  true,  true,  false, false, false, true,  true,  true};
bool PieceMin[13]      = {false, false, true,  true,  false, false, false, false, true,  true,  false, false, false};
int  PieceVal[13]      = {0,     100,   325,   325,   550,   1000,  50000, 100,   325,   325,   550,   1000,  50000};
int  PieceCol[13]      = {BOTH,  WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};

bool PiecePawn[13]        = {false, true,  false, false, false, false, false, true,  false, false, false, false, false};
bool PieceKnight[13]      = {false, false, true,  false, false, false, false, false, true,  false, false, false, false};
bool PieceKing[13]        = {false, false, false, false, false, false, true,  false, false, false, false, false, true};
bool PieceRookQueen[13]   = {false, false, false, false, true,  true,  false, false, false, false, true,  true,  false};
bool PieceBishopQueen[13] = {false, false, false, true,  false, true,  false, false, false, true,  false, true,  false};
bool PieceSlides[13]      = {false, false, false, true,  true,  true,  false, false, false, true,  true,  true,  false};

int Mirror64[64] = {
	56, 57, 58, 59, 60, 61, 62, 63,
	48, 49, 50, 51, 52, 53, 54, 55,
	40, 41, 42, 43, 44, 45, 46, 47,
	32, 33, 34, 35, 36, 37, 38, 39,
	24, 25, 26, 27, 28, 29, 30, 31,
	16, 17, 18, 19, 20, 21, 22, 23,
	 8,  9, 10, 11, 12, 13, 14, 15,
	 0,  1,  2,  3,  4,  5,  6,  7
};
// clang-format on
