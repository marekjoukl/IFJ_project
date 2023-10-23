CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic

all: scanner

scanner: scanner.o automat.o
	$(CC) $(CFLAGS) -o scanner scanner.o automat.o

scanner.o: scanner.c scanner.h
	$(CC) $(CFLAGS) -c scanner.c

automat.o: automat.c automat.h
	$(CC) $(CFLAGS) -c automat.c

clean:
	rm -f scanner scanner.o automat.o