#include "defs.h"
#include <assert.h>

static void CheckUp() {
    // check if time up, or interrupt from GUI
}

static int IsRepetition(const S_BOARD *pos) {

    int index = 0;

    for (index = pos->hisPly - pos->fiftyMove; index < pos->hisPly - 1; ++index) {
        assert(index >= 0 && index < MAXGAMEMOVES);
        if (pos->posKey == pos->history[index].posKey) {
            return true;
        }
    }
    return false;
}

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {

}

// factor in horizon-effect
static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {
    return 0;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull) {
    return 0;
}

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
    // iterative deepening, search init
}
