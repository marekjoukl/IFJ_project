/** 
 * Implementace překladače imperativního jazyka IFJ23
 * xputis00 Daniel Puts
 * xzelen29  Jakub Zelenay
 * xjoukl00 Marek Joukul
 * xkozan01 Ondrej Kožanyi
*/

#ifndef PRECEDENT_STACK_H
#define PRECEDENT_STACK_H

#include "symtable.h"
#include "symtable_stack.h"
#include <stdbool.h>

#define ERROR_HANDLE_PREC_STACK(exit_code) \
    fprintf(stderr, "Error: precedent_stack.c - error code %d\n", exit_code); \
    exit(exit_code);

typedef struct ast{
    char *data;
    data_type_t type;
    bool is_lit;
    struct ast *left;
    struct ast *right;
} ast_t;
typedef struct valid_itmes
{
    int type;
    data_type_t var_type;
    bool can_be_nil;
    bool is_lit;
    char* posfix_name;
    ast_t *tree;
}valid_itmes_t;

typedef struct prec_stack
{
    valid_itmes_t items;
    struct prec_stack *next;
}prec_stack_t;

void stack_init(prec_stack_t **stack);
bool stack_empty(prec_stack_t *stack);
void stack_push(prec_stack_t **stack, valid_itmes_t *item);
bool stack_top(prec_stack_t *stack, valid_itmes_t *dst);
void stack_top_terminal(prec_stack_t *stack, valid_itmes_t *dst);
void stack_pop(prec_stack_t **stack);
void stack_push_stoppage(prec_stack_t **stack);
bool check_stoppage(prec_stack_t *stack, int cnt);
bool rule1(prec_stack_t *stack, valid_itmes_t rule);
bool rule3(prec_stack_t *stack, valid_itmes_t rule);
void stack_merge(prec_stack_t **stack, valid_itmes_t new_expression);
void stack_dispose(prec_stack_t **stack);

void tree_init(ast_t **tree);
void tree_link(ast_t **parent, ast_t *left, ast_t *right);
void tree_insert(ast_t **tree, char *data);
void tree_postorder(ast_t *tree);
void tree_dispose(ast_t **tree);

#endif
