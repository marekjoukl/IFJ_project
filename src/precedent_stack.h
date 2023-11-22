#ifndef PRECEDENT_STACK_H
#define PRECEDENT_STACK_H

//#include "precedent.h"


//enum item_types
//{
//    TERMINAL,
//    EXPRESSION,
//    STOPPAGE,
//    DOLLAR
//};

typedef struct valid_itmes
{
    //prec_terminal_t type;
    int type;
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
void stack_merge(prec_stack_t **stack);
void stack_dispose(prec_stack_t **stack);

#endif
