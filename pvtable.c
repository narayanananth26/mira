#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

S_HASHTABLE HashTable[1];

int GetPvLine(const int depth, S_BOARD *pos, const S_HASHTABLE *table) {
    assert(depth < MAXDEPTH);

    int move = ProbePvMove(pos, table);
    int count = 0;

    while (move != NOMOVE && count < depth) {
        assert(count < MAXDEPTH);

        if (MoveExists(pos, move)) {
            MakeMove(pos, move);
            pos->PvArray[count++] = move;
        } else
            break;

        move = ProbePvMove(pos, table);
    }

    while (pos->ply > 0) {
        TakeMove(pos);
    }

    return count;
}

const int PvSize = 0x100000 * 2;

void ClearHashTable(S_HASHTABLE *table) {

    S_HASHENTRY *tableEntry;

    for (tableEntry = table->pTable; tableEntry < table->pTable + table->numEntries; tableEntry++) {
        tableEntry->posKey = 0ULL;
        tableEntry->move = NOMOVE;
        tableEntry->depth = 0;
        tableEntry->score = 0;
        tableEntry->flags = 0;
        tableEntry->gen = 0;
    }
    table->newWrite = 0;
    table->currentGen = 0;
}

void InitHashTable(S_HASHTABLE *table, const int MB) {

    int HashSize = 0x100000 * MB;
    table->numEntries = HashSize / sizeof(S_HASHENTRY);
    table->numEntries -= 2;

    if (table->pTable != NULL) {
        free(table->pTable);
    }

    table->pTable = (S_HASHENTRY *)malloc(table->numEntries * sizeof(S_HASHENTRY));
    if (table->pTable == NULL) {
        printf("Hash Allocation Failed, trying %dMB...\n", MB / 2);
        InitHashTable(table, MB / 2);
    } else {
        ClearHashTable(table);
        printf("HashTable init complete with %d entries\n", table->numEntries);
    }
}

int ProbeHashEntry(S_BOARD *pos, S_HASHTABLE *table, int *move, int *score, int alpha, int beta, int depth) {

    int index = pos->posKey % table->numEntries;

    assert(index >= 0 && index <= table->numEntries - 1);
    assert(depth >= 1 && depth < MAXDEPTH);
    assert(alpha < beta);
    assert(alpha >= -AB_BOUND && alpha <= AB_BOUND);
    assert(beta >= -AB_BOUND && beta <= AB_BOUND);
    assert(pos->ply >= 0 && pos->ply < MAXDEPTH);

    if (table->pTable[index].posKey == pos->posKey) {
        *move = table->pTable[index].move;
        if (table->pTable[index].depth >= depth) {
            table->hit++;

            assert(table->pTable[index].depth >= 1 && table->pTable[index].depth < MAXDEPTH);
            assert(table->pTable[index].flags >= HFALPHA && table->pTable[index].flags <= HFEXACT);

            *score = table->pTable[index].score;
            if (*score > ISMATE)
                *score -= pos->ply;
            else if (*score < -ISMATE)
                *score += pos->ply;

            switch (table->pTable[index].flags) {

                assert(*score >= -AB_BOUND && *score <= AB_BOUND);

            case HFALPHA:
                if (*score <= alpha) {
                    *score = alpha;
                    return true;
                }
                break;
            case HFBETA:
                if (*score >= beta) {
                    *score = beta;
                    return true;
                }
                break;
            case HFEXACT:
                return true;
                break;
            default:
                assert(false);
                break;
            }
        }
    }

    return false;
}

void StoreHashEntry(S_BOARD *pos, S_HASHTABLE *table, const int move, int score, const int flags, const int depth) {

    int index = pos->posKey % table->numEntries;

    assert(index >= 0 && index <= table->numEntries - 1);
    assert(depth >= 1 && depth < MAXDEPTH);
    assert(flags >= HFALPHA && flags <= HFEXACT);
    assert(score >= -AB_BOUND && score <= AB_BOUND);
    assert(pos->ply >= 0 && pos->ply < MAXDEPTH);

    // replace only if gen/depth is greater
    int replace = false;

    if (table->pTable[index].posKey == 0) {
        table->newWrite++;
        replace = true;
    } else if (table->pTable[index].gen < table->currentGen || table->pTable[index].depth <= depth) {
        replace = true;
    }

    if (replace == false) {
        return;
    }

    if (score > ISMATE)
        score += pos->ply;
    else if (score < -ISMATE)
        score -= pos->ply;

    table->pTable[index].move = move;
    table->pTable[index].posKey = pos->posKey;
    table->pTable[index].flags = flags;
    table->pTable[index].score = score;
    table->pTable[index].depth = depth;
    table->pTable[index].gen = table->currentGen;
}

int ProbePvMove(const S_BOARD *pos, const S_HASHTABLE *table) {
    int index = pos->posKey % table->numEntries;
    assert(index >= 0 && index <= table->numEntries - 1);

    if (table->pTable[index].posKey == pos->posKey) {
        return table->pTable[index].move;
    }

    return NOMOVE;
}
