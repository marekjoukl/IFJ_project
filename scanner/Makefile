CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic

all: test clean

scanner.o: scanner.c scanner.h automat.h
	$(CC) $(CFLAGS) -c scanner.c

automat.o: automat.c automat.h
	$(CC) $(CFLAGS) -c automat.c

test: test.o scanner.o automat.o
	$(CC) $(CFLAGS) -o test test.o scanner.o automat.o

test.o: test.c scanner.h automat.h
	$(CC) $(CFLAGS) -c test.c

clean:
	rm -f test.o scanner.o automat.o