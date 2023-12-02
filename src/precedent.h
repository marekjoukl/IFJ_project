#ifndef PRECEDENT_H
#define PRECEDENT_H

#include "error.h"
#include "scanner.h"
#include "precedent_stack.h"
#include "symtable.h"
#include "symtable_stack.h"

#define ERROR_HANDLE_PREC(exit_code, token) \
    fprintf(stderr, "Error: precedent.c - error code %d at line %d, with token: %d\n", exit_code, token->line, token->kind); \
    exit(exit_code);

typedef enum {
    MUL_T,
    DIV_T,
    MINUS_T,
    PLUS_T,
    EQUAL_T,
    NOT_EQUAL_T,
    LESS_T,
    GREATER_T,
    LESS_EQUAL_T,
    GREATER_EQUAL_T,        //
    DOUBLE_QUESTION_MARK_T, //
    EXCLAMATION_POINT_T,    //
    LEFT_PAR_T,             // 
    RIGHT_PAR_T,
    TERM_T,                 //
//    IDENTIFIER_T,
//    STRING_LIT_T,
//    INTEGER_LIT_T,
//    DOUBLE_LIT_T,
    DOLLAR_T,               //
    TERMINAL_CNT_T,
    EXPRESSION_T,   // not a terminal
    STOPPAGE_T      // not a terminal (stoppage in stack <)
} prec_terminal_t;

typedef enum stack_rules{
    ERROR_R,
    SHIFT_R,
    STOPPAGE_R,
    MERGE_R
} stack_rules_t;

extern const stack_rules_t prec_table[TERMINAL_CNT_T][TERMINAL_CNT_T];

stack_rules_t give_stack_rule(prec_stack_t *stack, prec_terminal_t input);
valid_itmes_t convert_lex_term(Lexeme lex, symtable_stack_t *sym_stack);
bool check_prec_rule(prec_stack_t *stack, valid_itmes_t *new_expression, Lexeme *token);
data_type_t precedent_analysys(Lexeme *lexeme, symtable_stack_t *sym_stack);

#endif // PRECEDENT_H
