SRCS = $(wildcard *.c)
NAME = mira

BOOK_PATH ?= performance.bin

all:
	gcc $(SRCS) -O2 -DNDEBUG -DMIRA_BOOK_PATH='"$(BOOK_PATH)"' -pthread -o $(NAME)
debug:
	gcc $(SRCS) -O0 -g -DMIRA_BOOK_PATH='"$(BOOK_PATH)"' -pthread -o $(NAME)
