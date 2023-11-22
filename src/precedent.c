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
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R , STOPPAGE_R, ERROR_R}  // $   
};

prec_rules_t give_rule(prec_stack_t *stack, prec_terminal_t input)
{
    valid_itmes_t top;
    stack_top_terminal(stack, &top);
    return prec_table[top.type][input];
}

bool precedent_analysys(Lexeme *lexeme)
{
    bool valid = true;
    bool cont = true;
    valid_itmes_t top; //debug
    prec_stack_t *stack;
    stack_init(&stack);
    //Lexeme lexeme = get_next_non_whitespace_lexeme();
    valid_itmes_t input = convert_lex_term(*lexeme);
    //puts("init"); //debug
    while(valid)
    {
        stack_top_terminal(stack, &top); //debug
        prec_rules_t rule = give_rule(stack, input.type);
        printf("1. top = %d, next = %d, rule = %d\n", top.type , input.type , rule); //debug
        switch (rule)
        {
        case SHIFT_R:
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
            stack_merge(&stack);
            break;
        case ERROR_R: 
            puts("ERROR"); //debug
            valid = false;
            cont = true;
            break;
//        case FINISH_R:
//            puts("FINISH"); //debug
//            valid = false;
//            cont = false;
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
        prec_rules_t rule = give_rule(stack, input.type);
        printf("2. top = %d, next = %d, rule = %d\n", top.type , input.type , rule); //debug
        switch (rule)
        {
        case MERGE_R:
            puts("MERGE"); //debug
            stack_merge(&stack);
            break;
//        case FINISH_R:
//            cont = false;
//            break;
        default:
            cont = false;
            break;
        }
    }

    if(stack_empty(stack))
        valid = true;
    
    stack_dispose(&stack);    
    return valid;
}