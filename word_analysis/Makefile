.PHONY:clean compile

CC=gcc
CFLAGS=-Wall -g

word_table.o:word_table.c
	$(CC) -c -o ../debug/$@ $< $(CFLAGS)

clean:
	rm -f ../debug/world_table.o
