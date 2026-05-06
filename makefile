all:
	gcc mira.c init.c bitboards.c hashkeys.c board.c data.c attack.c io.c movegen.c validate.c makemove.c -o mira
