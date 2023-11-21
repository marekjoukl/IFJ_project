CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -g

#scannaer files
scanner.o: scanner.c scanner.h automat.h
	$(CC) $(CFLAGS) -c scanner.c

automat.o: automat.c automat.h
	$(CC) $(CFLAGS) -c automat.c
test_scanner: test_scanner.o scanner.o automat.o
	$(CC) $(CFLAGS) -o test_scanner test_scanner.o scanner.o automat.o

test_scanner.o: test_scanner.c scanner.h automat.h
	$(CC) $(CFLAGS) -c test_scanner.c

#precedent files
precedent.o:
	$(CC) $(CFLAGS) -c precedent.c

precedent_stack.o: precedent_stack.c
	$(CC) $(CFLAGS) -c precedent_stack.c

test.o: test.c
	$(CC) $(CFLAGS) -c test.c

test: test.o precedent_stack.o precedent.o scanner.o automat.o
	$(CC) $(CFLAGS) -o test test.o precedent_stack.o precedent.o scanner.o automat.o

#clean
clean:
	rm test.o precedent_stack.o precedent.o scanner.o automat.o
