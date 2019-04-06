.PHONY:clean compile

CC=gcc
CFLAGS=-Wall -g

common_string.o:common_string.c
	$(CC) -c -o ../debug/$@ $< $(CFLAGS)

clean:
	rm -f ../debug/common_string.o
