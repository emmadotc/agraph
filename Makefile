FILES = main.c
LINKR = -lm -lncurses

default:
	gcc -Wall -std=c99 -march=native $(LINKR) $(FILES) -o agraph
