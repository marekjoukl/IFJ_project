/**
 * @file symtable_stack.h
 * @author Ondrej Kožányi (xkozan01)
 * @brief Header for symtable_stack
 */

#ifndef SYMTABLE_STACK_H
#define SYMTABLE_STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

#define STACK_INIT_SIZE 5

typedef struct symtable_stack {
    unsigned int capacity;
    unsigned int size;
    Symtable **array;
} symtable_stack_t;

symtable_stack_t* SymtableStackInit(void);  //initialize stack
void SymtableStackPush(symtable_stack_t *stack, Symtable *table);   //push symtable to stack
void SymtableStackPop(symtable_stack_t *stack); //pop symtable from stack
symtable_item_t* SymtableSearchAll(symtable_stack_t *stack, char *key); //search for item with given key in all symtables in stack
bool SymtableStackIsEmpty(symtable_stack_t *stack); //check if stack is empty
bool SymtableStackIsFull(symtable_stack_t *stack);  //check if stack is full
void SymtableStackDispose(symtable_stack_t *stack); //dispose stack


#endif //SYMTABLE_STACK_H
