CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -g

all: test_scanner

scanner.o: scanner.c scanner.h automat.h
	$(CC) $(CFLAGS) -c scanner.c

automat.o: automat.c automat.h
	$(CC) $(CFLAGS) -c automat.c

test_scanner: test_scanner.o scanner.o automat.o
	$(CC) $(CFLAGS) -o test_scanner test_scanner.o scanner.o automat.o

test_scanner.o: test_scanner.c scanner.h automat.h
	$(CC) $(CFLAGS) -c test_scanner.c

clean:
	rm -f test_scanner.o scanner.o automat.o test_scanner