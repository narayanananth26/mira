SRCS = mira.c init.c bitboards.c hashkeys.c board.c data.c attack.c io.c movegen.c validate.c makemove.c perft.c search.c misc.c pvtable.c evaluate.c uci.c xboard.c polybook.c polykeys.c

# Absolute path to the opening book for installed builds (Homebrew overrides
# this); defaults to the repo-local file for development.
BOOK_PATH ?= performance.bin

all:
	gcc $(SRCS) -O2 -DNDEBUG -DMIRA_BOOK_PATH='"$(BOOK_PATH)"' -o mira

debug:
	gcc $(SRCS) -O0 -g -DMIRA_BOOK_PATH='"$(BOOK_PATH)"' -o mira
