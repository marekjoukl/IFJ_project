#include "parser.h" 
#include <stdio.h>

int main(void){
    symtable_stack_t *stack = SymtableStackInit();

    CREATE_FRAME()
    StartParser(true, stack);
    StartParser(false, stack);

    SymtableStackPop(stack);
    SymtableStackDispose(stack);

    return 0;
}
