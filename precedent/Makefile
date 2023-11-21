CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -g

all: test

#scannaer files
SCANNER=../scanner/
scanner.o: $(SCANNER)scanner.c 
	$(CC) $(CFLAGS) -c $(SCANNER)scanner.c

automat.o: $(SCANNER)automat.c 
	$(CC) $(CFLAGS) -c $(SCANNER)automat.c

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
	