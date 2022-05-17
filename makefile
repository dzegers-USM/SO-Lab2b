CC = gcc
CFLAGS = -I.

make: main.c deck.c util.c
	$(CC) $(CFLAGS) -o main.elf main.c deck.c util.c
