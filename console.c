#include "defs.h"
#include <stdio.h>
#include <string.h>

static const char *Glyph[13] = {
    " ",
    "♟", "♞", "♝", "♜", "♛", "♚", // white
    "♟", "♞", "♝", "♜", "♛", "♚", // black
};

#define LIGHT_BG "\033[48;5;180m"
#define DARK_BG "\033[48;5;137m"
#define WHITE_FG "\033[38;5;231m"
#define BLACK_FG "\033[38;5;16m"
#define RESET "\033[0m"

void PrintBoardFor(const S_BOARD *pos, const int humanSide) {

    int sq, file, rank, piece;
    int rStart = RANK_8, rEnd = RANK_1, rStep = -1;
    int fStart = FILE_A, fEnd = FILE_H, fStep = 1;

    if (humanSide == BLACK) {
        rStart = RANK_1;
        rEnd = RANK_8;
        rStep = 1;

        fStart = FILE_H;
        fEnd = FILE_A;
        fStep = -1;
    }

    printf("\n");
    for (rank = rStart; rank != rEnd + rStep; rank += rStep) {
        printf(" %d ", rank + 1);
        for (file = fStart; file != fEnd + fStep; file += fStep) {
            sq = FR2SQ(file, rank);
            piece = pos->pieces[sq];

            const char *bg = ((file + rank) % 2) ? LIGHT_BG : DARK_BG;
            const char *fg = (piece >= bP) ? BLACK_FG : WHITE_FG;

            printf("%s%s\033[1m %s %s", bg, fg, Glyph[piece], RESET);
        }
        printf("\n");
    }

    printf("   ");
    for (file = fStart; file != fEnd + fStep; file += fStep) {
        printf(" %c ", 'a' + file);
    }
    printf("\n\n");
}

static int LegalMoveCount(S_BOARD *pos) {

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    int index = 0;
    int legal = 0;

    for (index = 0; index < list->count; ++index) {
        if (!MakeMove(pos, list->moves[index].move))
            continue;
        TakeMove(pos);
        legal++;
    }

    return legal;
}

static int RepCount(const S_BOARD *pos) {

    int index = 0;
    int count = 0;

    for (index = 0; index < pos->hisPly; ++index) {
        if (pos->history[index].posKey == pos->posKey)
            count++;
    }

    return count;
}

enum { ONGOING, CHECKMATE, STALEMATE, FIFTY, THREEFOLD };

static int GameStatus(S_BOARD *pos) {

    if (LegalMoveCount(pos) == 0) {
        if (SqAttacked(pos->KingSq[pos->side], pos->side ^ 1, pos))
            return CHECKMATE;
        return STALEMATE;
    }
    if (pos->fiftyMove >= 100)
        return FIFTY;
    if (RepCount(pos) >= 2)
        return THREEFOLD;

    return ONGOING;
}

// difficulty 1-5
static int Think(S_BOARD *pos, S_SEARCHINFO *info, int useTime, int value) {

    S_BOARD copy[1];
    memcpy(copy, pos, sizeof(S_BOARD));

    info->GAME_MODE = 1;
    info->stopped = 0;
    info->quit = 0;

    if (useTime) {
        info->timeset = true;
        info->starttime = GetTimeMs();
        info->stoptime = info->starttime + value;
        info->depth = MAXDEPTH;
    } else {
        info->timeset = false;
        info->depth = value;
    }

    SearchPosition(copy, info, HashTable);
    return copy->PvArray[0];
}

// {useTime, value}
static const int Levels[5][2] = {{0, 1}, {0, 3}, {0, 5}, {1, 1000}, {1, 3000}};

static void PrintLegal(S_BOARD *pos) {

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    int index = 0;
    int shown = 0;

    for (index = 0; index < list->count; ++index) {
        if (!MakeMove(pos, list->moves[index].move))
            continue;
        TakeMove(pos);
        printf("%s ", PrMove(list->moves[index].move));
        shown++;
    }
    printf("\n(%d legal moves)\n", shown);
}

static void PrintHelp(void) {
    printf("commands: <move> | undo | new | board | moves | hint | quit\n");
    printf("moves accept SAN (Nf3, exd5, O-O, e8=Q) or coords (e2e4)\n");
}

// clear screen + scrollback
static void ClearScreen(void) { printf("\033[2J\033[3J\033[H"); }

// score (white-relative cp)
static void FormatScore(char *buf, int size, int whiteCp) {
    if (whiteCp > ISMATE)
        snprintf(buf, size, "#%d", (INF_BOUND - whiteCp + 1) / 2);
    else if (whiteCp < -ISMATE)
        snprintf(buf, size, "#-%d", (INF_BOUND + whiteCp + 1) / 2);
    else
        snprintf(buf, size, "%+.2f", whiteCp / 100.0);
}

void ConsoleLoop(S_BOARD *pos, S_SEARCHINFO *info) {

    char line[256];
    char note[64] = "";
    int humanSide;
    int level = 3;
    int move;
    int redraw = 1;

    printf("\nmira console - let's play\n");

    printf("play as (w)hite, (b)lack or (r)andom? ");
    if (!fgets(line, sizeof(line), stdin))
        return;
    if (line[0] == 'b')
        humanSide = BLACK;
    else if (line[0] == 'r')
        humanSide = (GetTimeMs() & 1) ? WHITE : BLACK;
    else
        humanSide = WHITE;

    printf("difficulty (1-5)? ");
    if (!fgets(line, sizeof(line), stdin))
        return;
    if (line[0] >= '1' && line[0] <= '5')
        level = line[0] - '0';

    printf("you are %s, level %d\n", humanSide == WHITE ? "white" : "black", level);

    ParseFen(START_FEN, pos);

    while (true) {
        if (redraw) {
            ClearScreen();
            PrintBoardFor(pos, humanSide);
            if (note[0])
                printf("%s\n", note);
        }
        redraw = 1;

        int status = GameStatus(pos);
        if (status == CHECKMATE) {
            printf("checkmate - %s\n", pos->side == humanSide ? "mira wins" : "you win");
            break;
        } else if (status == STALEMATE) {
            printf("stalemate - draw\n");
            break;
        } else if (status == FIFTY) {
            printf("draw - fifty move rule\n");
            break;
        } else if (status == THREEFOLD) {
            printf("draw - threefold repetition\n");
            break;
        }

        // engine's turn
        if (pos->side != humanSide) {
            printf("mira is thinking...\n");
            int engineSide = pos->side;
            move = Think(pos, info, Levels[level - 1][0], Levels[level - 1][1]);
            if (move == NOMOVE)
                break;
            char san[16];
            strcpy(san, PrMoveSan(pos, move)); // SAN needs the pre-move position
            MakeMove(pos, move);
            if (info->bestScore == NOSCORE) {
                snprintf(note, sizeof(note), "mira plays %s   (book)", san);
            } else {
                char ev[16];
                FormatScore(ev, sizeof(ev), engineSide == WHITE ? info->bestScore : -info->bestScore);
                snprintf(note, sizeof(note), "mira plays %s   eval %s", san, ev);
            }
            continue;
        }

        // human's turn
        printf("your move: ");
        if (!fgets(line, sizeof(line), stdin))
            break;
        if (line[0] == '\n') {
            redraw = 0;
            continue;
        }

        if (!strncmp(line, "quit", 4) || !strncmp(line, "exit", 4)) {
            break;
        } else if (!strncmp(line, "help", 4) || line[0] == '?') {
            PrintHelp();
            redraw = 0;
            continue;
        } else if (!strncmp(line, "board", 5)) {
            continue;
        } else if (!strncmp(line, "moves", 5)) {
            PrintLegal(pos);
            redraw = 0;
            continue;
        } else if (!strncmp(line, "new", 3)) {
            ParseFen(START_FEN, pos);
            note[0] = '\0';
            continue;
        } else if (!strncmp(line, "hint", 4)) {
            move = Think(pos, info, 0, 4);
            char san[16];
            strcpy(san, PrMoveSan(pos, move));
            if (info->bestScore == NOSCORE) {
                snprintf(note, sizeof(note), "hint: %s   (book)", san);
            } else {
                char ev[16];
                FormatScore(ev, sizeof(ev), pos->side == WHITE ? info->bestScore : -info->bestScore);
                snprintf(note, sizeof(note), "hint: %s   eval %s", san, ev);
            }
            continue;
        } else if (!strncmp(line, "undo", 4) || line[0] == 'u') {
            int back = pos->hisPly >= 2 ? 2 : pos->hisPly;
            while (back-- > 0)
                TakeMove(pos);
            note[0] = '\0';
            continue;
        }

        move = ParseSan(line, pos);
        if (move == NOMOVE) {
            snprintf(note, sizeof(note), "illegal or unknown move");
            continue;
        }
        MakeMove(pos, move);
        note[0] = '\0';
    }
}
