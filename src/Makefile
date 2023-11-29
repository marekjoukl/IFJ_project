CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -g

#scanner files
scanner.o: scanner.c scanner.h automat.h
	$(CC) $(CFLAGS) -c scanner.c

automat.o: automat.c automat.h
	$(CC) $(CFLAGS) -c automat.c
test_scanner: test_scanner.o scanner.o automat.o
	$(CC) $(CFLAGS) -o test_scanner test_scanner.o scanner.o automat.o

test_scanner.o: test_scanner.c scanner.h automat.h
	$(CC) $(CFLAGS) -c test_scanner.c

#parser files
parser.o: precedent.c precedent.h parser.c parser.h scanner.c automat.c
	$(CC) $(CFLAGS) -c parser.c

test_parser.o: test_parser.c
	$(CC) $(CFLAGS) -c test_parser.c

test_parser: test_parser.o parser.o automat.o scanner.o precedent.o precedent_stack.o
	$(CC) $(CFLAGS) -o test_parser test_parser.o parser.o scanner.o automat.o precedent.o precedent_stack.o

#symtable files
symtable.o: symtable.c symtable.h precedent.c precedent.h parser.c parser.h error.h
	$(CC) $(CFLAGS) -c symtable.c

test_symtable.o: test_symtable.c symtable.h parser.h error.h scanner.h
	$(CC) $(CFLAGS) -c test_symtable.c

test_symtable: test_symtable.o symtable.o parser.o automat.o scanner.o precedent.o precedent_stack.o 
	$(CC) $(CFLAGS) -o test_symtable test_symtable.o symtable.o parser.o scanner.o automat.o precedent.o precedent_stack.o

#precedent files
precedent.o: precedent.c
	$(CC) $(CFLAGS) -c precedent.c

precedent_stack.o: precedent_stack.c
	$(CC) $(CFLAGS) -c precedent_stack.c

test_precedent.o: test_precedent.c
	$(CC) $(CFLAGS) -c test_precedent.c

test_precedent: test_precedent.o precedent_stack.o precedent.o scanner.o automat.o
	$(CC) $(CFLAGS) -o test_precedent test_precedent.o precedent_stack.o precedent.o scanner.o automat.o

#clean
clean:
	rm -f *.o test_scanner test_parser test test_symtable
