SRCS = mira.c init.c bitboards.c hashkeys.c board.c data.c attack.c io.c movegen.c validate.c makemove.c perft.c search.c misc.c

all:
	gcc $(SRCS) -O2 -DNDEBUG -o mira

debug:
	gcc $(SRCS) -O0 -g -o mira
