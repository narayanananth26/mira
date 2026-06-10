#include "defs.h"
#include <assert.h>

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
