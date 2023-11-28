#include "precedent.h"
//#include "precedent_stack.h"
#include "scanner.h"


//debug ///////////////////////////////////////////////////////////////////////
void print_stack2(prec_stack_t *stack)
{
    int i = 0;
    printf("------------stack-----------\n");
    while(stack != NULL)
    {
        printf("position %d, type = %d\n", i, stack->items.type);
        stack = stack->next;
        i++;
    }
    printf("----------------------------\n");
}
/////////////////////////////////////////////////////////////////////////////////

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

const stack_rules_t prec_table[TERMINAL_CNT_T][TERMINAL_CNT_T] = 
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
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R , STOPPAGE_R, ERROR_R}  // $   
};


stack_rules_t give_stack_rule(prec_stack_t *stack, prec_terminal_t input)
{
    valid_itmes_t top;
    stack_top_terminal(stack, &top);
    return prec_table[top.type][input];
}

bool precedent_analysys(Lexeme *lexeme)
{
    bool valid = true;
    bool cont = true;
    valid_itmes_t top; //
    prec_stack_t *stack;
    stack_init(&stack);
    valid_itmes_t input = convert_lex_term(*lexeme);
    stack_rules_t stack_rule;

    while(valid)
    {
        stack_top_terminal(stack, &top); //debug
        stack_rule = give_stack_rule(stack, input.type);
        printf("1. top = %d, next = %d, stack_rule = %d\n", top.type , input.type , stack_rule); //debug
        switch (stack_rule)
        {
        case SHIFT_R:
            puts("SHIFT"); //debug
            stack_push(&stack, &input);
            *lexeme = get_next_non_whitespace_lexeme();
            input = convert_lex_term(*lexeme);
            break;
        case STOPPAGE_R:
            puts("STOPPAGE"); //debug
            stack_push_stoppage(&stack);
            stack_push(&stack, &input);
            *lexeme = get_next_non_whitespace_lexeme();
            input = convert_lex_term(*lexeme);
            break;
        case MERGE_R:
            puts("MERGE"); //debug
            if(check_prec_rule(stack))    
                stack_merge(&stack);
            else
            {
                valid = false;
                cont = false;
            }
            break;
        case ERROR_R: 
            puts("ERROR"); //debug
            valid = false;
            cont = true;
            break;
        default:
            break;
        }
    }
    print_stack2(stack); //debug

    // second traverse with input = $
    input.type = DOLLAR_T;
    while(cont == true)
    {
        stack_top_terminal(stack, &top); //debug
        stack_rules_t stack_rule = give_stack_rule(stack, input.type);
        printf("2. top = %d, next = %d, stack_rule = %d\n", top.type , input.type , stack_rule); //debug
        switch (stack_rule)
        {
        case MERGE_R:
            puts("MERGE2"); //debug
            if(check_prec_rule(stack))    
                stack_merge(&stack);
            else
                valid = false;
            break;
        default:
            cont = false;
            break;
        }
    }

    if(stack_empty(stack))
        valid = true;
//    else
//        exit(2);
    
    stack_dispose(&stack);    
    return valid;
}