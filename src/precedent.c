#include "precedent.h"
#include "scanner.h"

valid_itmes_t convert_lex_term(Lexeme lex)
{
    valid_itmes_t item;
    switch (lex.kind)
    {
        case LESS:                  item.type = LESS_T; break;
        case LESS_EQUAL:            item.type = LESS_EQUAL_T; break;
        case GREATER:               item.type = GREATER_T; break;
        case GREATER_EQUAL:         item.type = GREATER_EQUAL_T; break;
        case EXCLAMATION_POINT:     item.type = EXCLAMATION_POINT_T; break;
        case NOT_EQUAL:             item.type = NOT_EQUAL_T; break;
        case EQUAL:                 item.type = EQUAL_T; break;
        case RIGHT_PAR:             item.type = RIGHT_PAR_T; break;
        case LEFT_PAR:              item.type = LEFT_PAR_T; break;
        case DOUBLE_QUESTION_MARK:  item.type = DOUBLE_QUESTION_MARK_T; break;
        case IDENTIFIER:            item.type = TERM_T; break;
        case STRING_LIT:            item.type = TERM_T; break;
        case MULTILINE_STRING_LIT:  item.type = TERM_T; break;
        case INTEGER_LIT:           item.type = TERM_T; break;
        case DOUBLE_LIT:            item.type = TERM_T; break;
        case PLUS:                  item.type = PLUS_T; break;
        case MINUS:                 item.type = MINUS_T; break;
        case ASTERISK:              item.type = MUL_T; break;
        case SLASH:                 item.type = DIV_T; break;
        default: item.type = DOLLAR_T; break;
    }
    return item;
}

prec_rules_t give_rule(prec_stack_t *stack, prec_terminal_t input)
{
    valid_itmes_t top;
    stack_top_terminal(stack, &top);
    //input = MUL_T; //debug
    return prec_table[input][top.type]; //debug
}

bool precedent_analysys()
{
    bool valid = true;
    prec_stack_t *stack;
    stack_init(&stack);
    Lexeme lexeme = get_next_non_whitespace_lexeme();
    valid_itmes_t input = convert_lex_term(lexeme);

    while(valid)
    {
        prec_rules_t rule = give_rule(stack, input.type);
        switch (rule)
        {
        case SHIFT_R:
            stack_push(&stack, &input);
            lexeme = get_next_non_whitespace_lexeme();
            input = convert_lex_term(lexeme);
            break;
        case STOPPAGE_R:
            stack_push_stoppage(&stack);
            stack_push(&stack, &input);
            lexeme = get_next_non_whitespace_lexeme();
            input = convert_lex_term(lexeme);
            break;
        case MERGE_R:
            stack_merge(&stack);
            break;
        case ERROR_R:
            valid = false;
        default:
            break;
        }
    }
    if(stack_empty(stack))
        valid = true;
    stack_dispose(&stack);    
    return valid;
}