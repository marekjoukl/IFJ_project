/**
 * @file symtable.c
 * @author Ondrej Kožányi (xkozan01)
 * @brief stack to store symtables
 */

#include "symtable_stack.h"

symtable_stack_t* SymtableStackInit() {
    symtable_stack_t *stack = malloc(sizeof(symtable_stack_t));
    if (stack == NULL) {
        fprintf(stderr, "Error: symtable_stack.c - SymtableStackInit() - malloc failed\n");
        exit(INTERNAL_ERROR);
    }
    stack->capacity = STACK_INIT_SIZE;
    stack->size = 0;
    stack->array = malloc(sizeof(Symtable) * stack->capacity);
    if (stack->array == NULL) {
        fprintf(stderr, "Error: symtable_stack.c - SymtableStackInit() - malloc failed\n");
        exit(INTERNAL_ERROR);
    }
    return stack;
}

void SymtableStackPush(symtable_stack_t *stack, Symtable *table) {
    if (SymtableStackIsFull(stack)) {
        stack->capacity += STACK_INIT_SIZE;
        stack->array = realloc(stack->array, sizeof(Symtable) * stack->capacity);
        if (stack->array == NULL) {
            fprintf(stderr, "Error: symtable_stack.c - SymtableStackPush() - realloc failed\n");
            exit(INTERNAL_ERROR);
        }
    }
    stack->array[stack->size] = table;
    stack->size++;
}

void SymtableStackPop(symtable_stack_t *stack) {
    if (SymtableStackIsEmpty(stack)) {
        fprintf(stderr, "Error: symtable_stack.c - SymtableStackPop() - stack is empty\n");
        exit(INTERNAL_ERROR);
    }
    SymtableDeleteAll(stack->array[stack->size - 1]);
    free(stack->array[stack->size - 1]);
    stack->size--;
}

symtable_item_t *SymtableSearchAll(symtable_stack_t *stack, char *key) {
    symtable_item_t *item = NULL;
    for (int i = stack->size - 1; i >= 0; i--) {
        item = SymtableSearch(stack->array[i], key);
        if (item != NULL) {
            return item;
        }
    }
    return NULL;
}

bool SymtableStackIsEmpty(symtable_stack_t *stack) {
    return stack->size == 0;
}

bool SymtableStackIsFull(symtable_stack_t *stack) {
    return stack->size >= stack->capacity;
}

void SymtableStackDispose(symtable_stack_t *stack) {
    for (unsigned int i = 0; i < stack->size; i++) {
        SymtableDeleteAll(stack->array[i]);
    }
    free(stack->array);
    free(stack);
}

