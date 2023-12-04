#include "../src/parser.h"
#include <stdio.h>

int main(int argc, char *argv[]){

    FILE *input = stdin;
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (input == NULL) {
            printf("Could not open file %s\n", argv[1]);
             return 1;
        }
    }
    symtable_stack_t *stack = SymtableStackInit();

    CREATE_FRAME()
    StartParser(true, stack);
    StartParser(false, stack);

    SymtableStackPop(stack);
    SymtableStackDispose(stack);

    printf("funguje\n");
    fclose(input);
    return 0;
}
