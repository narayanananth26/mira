#include "defs.h"

char PceChar[] = ".PNBRQKpnbrqk";
char SideChar[] = "wb-";
char RankChar[] = "12345678";
char FileChar[] = "abcdefgh";

//                  EMPTY  P      N     B     R     Q     K     p      n     b     r     q     k
bool PieceBig[13] = {false, false, true, true, true, true, true, false, true, true, true, true, true};
bool PieceMaj[13] = {false, false, false, false, true, true, true, false, false, false, true, true, true};
bool PieceMin[13] = {false, false, true, true, false, false, false, false, true, true, false, false, false};
int PieceVal[13] = {0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000};
int PieceCol[13] = {BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};

bool PiecePawn[13] = {false, true, false, false, false, false, false, true, false, false, false, false, false};
bool PieceKnight[13] = {false, false, true, false, false, false, false, false, true, false, false, false, false};
bool PieceKing[13] = {false, false, false, false, false, false, true, false, false, false, false, false, true};
bool PieceRookQueen[13] = {false, false, false, false, true, true, false, false, false, false, true, true, false};
bool PieceBishopQueen[13] = {false, false, false, true, false, true, false, false, false, true, false, true, false};
bool PieceSlides[13] = {false, false, false, true, true, true, false, false, false, true, true, true, false};
