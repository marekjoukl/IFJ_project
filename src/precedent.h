#ifndef PRECEDENT_H
#define PRECEDENT_H

//#include <stdio.h>
#include "../scanner/scanner.h"
#include "precedent_stack.h"

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

typedef enum prec_rules{
    ERROR_R,
    SHIFT_R,
    STOPPAGE_R,
    MERGE_R
} prec_rules_t;

const prec_rules_t prec_table[TERMINAL_CNT_T][TERMINAL_CNT_T] = 
{ // +           -           *           /           ==          !=          <           >           <=          >=          ??          !           (           )         term        $
    {MERGE_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // +
    {MERGE_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // -
    {MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // *
    {MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // /
    {ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // ==
    {ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // !=
    {ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // <  
    {ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // >
    {ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // <=
    {ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // >=
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // ??
    {MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , MERGE_R   , ERROR_R   , ERROR_R   , MERGE_R , ERROR_R   , MERGE_R}, // !
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, SHIFT_R , STOPPAGE_R, ERROR_R}, // (
    {MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , ERROR_R   , MERGE_R , ERROR_R   , MERGE_R}, // )
    {MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , ERROR_R   , MERGE_R , ERROR_R   , MERGE_R}, // term
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R , STOPPAGE_R, ERROR_R} // $   
};

prec_rules_t give_rule(prec_stack_t *stack, prec_terminal_t input);
valid_itmes_t convert_lex_term(Lexeme lex);
bool precedent_analysys();

#endif // PRECEDENT_H