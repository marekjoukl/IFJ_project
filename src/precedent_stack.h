#ifndef PRECEDENT_STACK_H
#define PRECEDENT_STACK_H

//#include "precedent.h"
#include "symtable.h"
#include "symtable_stack.h"
#include <stdbool.h>

typedef struct valid_itmes
{
    int type;
    data_type_t var_type;
    bool can_be_nil;
    bool is_lit;
}valid_itmes_t;

typedef struct prec_stack
{
    valid_itmes_t items;
    struct prec_stack *next;
}prec_stack_t;

void stack_init(prec_stack_t **stack);
bool stack_empty(prec_stack_t *stack);
void stack_push(prec_stack_t **stack, valid_itmes_t *item);
void stack_top_terminal(prec_stack_t *stack, valid_itmes_t *destination);
void stack_push_stoppage(prec_stack_t **stack);
bool check_stoppage(prec_stack_t *stack, int cnt);
bool rule1(prec_stack_t *stack, valid_itmes_t rule);
bool rule3(prec_stack_t *stack, valid_itmes_t rule);
void stack_merge(prec_stack_t **stack, valid_itmes_t new_expression);
void stack_dispose(prec_stack_t **stack);

#endif
