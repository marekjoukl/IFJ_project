CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -g

main: main.o scanner.o automat.o parser.o precedent.o precedent_stack.o symtable.o symtable_stack.o
	$(CC) $(CFLAGS) -o main main.o scanner.o automat.o parser.o precedent.o precedent_stack.o symtable.o symtable_stack.o

main.o: main.c

#scanner files
scanner.o: scanner.c
	$(CC) $(CFLAGS) -c scanner.c

automat.o: automat.c
	$(CC) $(CFLAGS) -c automat.c

test_scanner.o: ../tests/test_scanner.c
	$(CC) $(CFLAGS) -c ../tests/test_scanner.c

test_scanner: test_scanner.o scanner.o automat.o
	$(CC) $(CFLAGS) -o test_scanner test_scanner.o scanner.o automat.o

#parser files
parser.o: precedent.c
	$(CC) $(CFLAGS) -c parser.c

test_parser.o: ../tests/test_parser.c
	$(CC) $(CFLAGS) -c ../tests/test_parser.c

test_parser: clean test_parser.o parser.o scanner.o precedent.o precedent_stack.o symtable.o symtable_stack.o automat.o generator.o builtin_func.o
	$(CC) $(CFLAGS) -o test_parser test_parser.o parser.o scanner.o precedent.o precedent_stack.o symtable.o symtable_stack.o automat.o generator.o builtin_func.o

#symtable files
symtable.o: symtable.c
	$(CC) $(CFLAGS) -c symtable.c

test_symtable.o: ../tests/test_symtable.c
	$(CC) $(CFLAGS) -c ../tests/test_symtable.c

test_symtable: test_symtable.o symtable.o automat.o scanner.o
	$(CC) $(CFLAGS) -o test_symtable test_symtable.o symtable.o automat.o scanner.o

#symtable stack files
symtable_stack.o: symtable_stack.c
	$(CC) $(CFLAGS) -c symtable_stack.c

#precedent files
precedent.o: precedent.c
	$(CC) $(CFLAGS) -c precedent.c

precedent_stack.o: precedent_stack.c
	$(CC) $(CFLAGS) -c precedent_stack.c

test_precedent.o: ../tests/test_precedent.c
	$(CC) $(CFLAGS) -c ../tests/test_precedent.c

test_precedent: test_precedent.o precedent_stack.o precedent.o scanner.o automat.o symtable.o symtable_stack.o
	$(CC) $(CFLAGS) -o test_precedent test_precedent.o precedent_stack.o precedent.o scanner.o automat.o symtable.o symtable_stack.o

#generator files
generator.o: generator.c
	$(CC) $(CFLAGS) -c generator.c

builtin_func.o: builtin_func.c
	$(CC) $(CFLAGS) -c builtin_func.c

test_generator.o: ../tests/test_generator.c
	$(CC) $(CFLAGS) -c ../tests/test_generator.c

test_generator: test_generator.o generator.o builtin_func.o scanner.o automat.o symtable.o symtable_stack.o precedent.o precedent_stack.o
	$(CC) $(CFLAGS) -o test_generator test_generator.o generator.o builtin_func.o scanner.o automat.o symtable.o symtable_stack.o precedent.o precedent_stack.o

#clean
clean:
	rm -f *.o test_scanner test_parser test_precedent test_symtable test_generator
