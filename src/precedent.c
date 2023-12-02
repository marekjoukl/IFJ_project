#include "precedent.h"
#include "scanner.h"

// TODO a + b == c is valid expression

valid_itmes_t convert_lex_term(Lexeme lex, symtable_stack_t *sym_stack)
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
        case PLUS:                  item.type = PLUS_T; break;
        case MINUS:                 item.type = MINUS_T; break;
        case ASTERISK:              item.type = MUL_T; break;
        case SLASH:                 item.type = DIV_T; break;

        case IDENTIFIER:
            item.type = TERM_T; 

            symtable_item_t *variable = SymtableSearchAll(sym_stack, lex.extra_data.string);
            item.var_type = variable->data->item_type;
            item.can_be_nil = variable->data->can_be_nil;
            item.is_lit = false;
            break;
        
        case STRING_LIT:
            item.type = TERM_T; 
            item.var_type = TYPE_STRING;
            item.can_be_nil = false;
            item.is_lit = true;
            break;

        case INTEGER_LIT:           
            item.type = TERM_T;
            item.var_type = TYPE_INT;
            item.can_be_nil = false;
            item.is_lit = true;
            break;

        case DOUBLE_LIT:
            item.type = TERM_T;
            item.var_type = TYPE_DOUBLE;
            item.can_be_nil = false;
            item.is_lit = true;
            break;

        case NIL:
            item.type = TERM_T;
            item.var_type = TYPE_NIL;
            item.can_be_nil = true;
            item.is_lit = true;
            break;

        default: item.type = DOLLAR_T; break;
    }
    return item;
}

const stack_rules_t prec_table[TERMINAL_CNT_T][TERMINAL_CNT_T] = 
{ // +           -           *           /           ==          !=          <           >           <=          >=          ??          !           (           )         term        $
    {MERGE_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // +
    {MERGE_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // -
    {MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // *
    {MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // /
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // ==
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // !=
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // <  
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // >
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // <=
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // >=
    {STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , ERROR_R   , STOPPAGE_R, STOPPAGE_R, STOPPAGE_R, MERGE_R , STOPPAGE_R, MERGE_R}, // ??
    {MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , MERGE_R   , ERROR_R   , ERROR_R   , MERGE_R , ERROR_R   , MERGE_R}, // !
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

bool check_prec_rule(prec_stack_t *stack, valid_itmes_t *new_expression, Lexeme *token)
{
    valid_itmes_t rule;
    stack_top_terminal(stack, &rule);
    bool valid = false;
    new_expression->type = EXPRESSION_T;

    switch (rule.type)
    {
    case MUL_T:
        valid = rule3(stack, rule);
        if((stack->items.var_type != TYPE_INT && stack->items.var_type != TYPE_DOUBLE) || 
            (stack->next->next->items.var_type != TYPE_INT && stack->next->next->items.var_type != TYPE_DOUBLE))
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        new_expression->var_type = stack->items.var_type;
        new_expression->can_be_nil = false;

        if(stack->items.var_type != stack->next->next->items.var_type) //TODO
        {
            if(stack->items.var_type == TYPE_INT){
                if(stack->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }
            
            if(stack->next->next->items.var_type == TYPE_INT){

                if(stack->next->next->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }

            new_expression->var_type = TYPE_DOUBLE;
        }

        if(stack->items.is_lit == false)
            new_expression->is_lit = stack->items.is_lit;
        else
            new_expression->is_lit = stack->next->next->items.is_lit;

        break;
    
    case DIV_T:
        valid = rule3(stack, rule);    
        if((stack->items.var_type != TYPE_INT && stack->items.var_type != TYPE_DOUBLE) || 
            (stack->next->next->items.var_type != TYPE_INT && stack->next->next->items.var_type != TYPE_DOUBLE))
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if (stack->items.var_type != stack->next->next->items.var_type)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        new_expression->can_be_nil = false;
        new_expression->var_type = stack->items.var_type;

        if(stack->items.is_lit == false)
            new_expression->is_lit = stack->items.is_lit;
        else
            new_expression->is_lit = stack->next->next->items.is_lit;

        break;
    
    case PLUS_T:
        valid = rule3(stack, rule);
        if(stack->items.var_type == TYPE_BOOL || stack->items.var_type == TYPE_UNDEFINED ||
           stack->next->next->items.var_type == TYPE_BOOL || stack->next->next->items.var_type == TYPE_UNDEFINED)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        new_expression->can_be_nil = false;
        new_expression->var_type = stack->items.var_type;

        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            if(stack->items.var_type == TYPE_STRING)
                {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

            if(stack->items.var_type == TYPE_INT){
                if(stack->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }

            if(stack->next->next->items.var_type == TYPE_INT){
                if(stack->next->next->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }

            new_expression->var_type = TYPE_DOUBLE;
        }
        
        if(stack->items.is_lit == false)
            new_expression->is_lit = stack->items.is_lit;
        else
            new_expression->is_lit = stack->next->next->items.is_lit;
        break;
    
    case MINUS_T:
        valid = rule3(stack, rule);

        if((stack->items.var_type != TYPE_INT && stack->items.var_type != TYPE_DOUBLE) || 
            (stack->next->next->items.var_type != TYPE_INT && stack->next->next->items.var_type != TYPE_DOUBLE))
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        new_expression->can_be_nil = false;
        new_expression->var_type = stack->items.var_type;

        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            if(stack->items.var_type == TYPE_INT){
                if(stack->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }
            
            if(stack->next->next->items.var_type == TYPE_INT){
                if(stack->next->next->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }

            new_expression->var_type = TYPE_DOUBLE;
        }

        if(stack->items.is_lit == false)
            new_expression->is_lit = stack->items.is_lit;
        else
            new_expression->is_lit = stack->next->next->items.is_lit;

        break;
    
    case EQUAL_T:
        valid = rule3(stack, rule);    
        if(stack->items.var_type == TYPE_BOOL || stack->items.var_type == TYPE_UNDEFINED)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            if(stack->items.var_type == TYPE_STRING || stack->next->next->items.var_type == TYPE_STRING)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

            if(stack->items.var_type == TYPE_INT){
                if(stack->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }
            
            if(stack->next->next->items.var_type == TYPE_INT){
                if(stack->next->next->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }
        }

        new_expression->can_be_nil = false;
        new_expression->var_type = TYPE_BOOL;
        break;
    
    case NOT_EQUAL_T:
        valid = rule3(stack, rule);
        if(stack->items.var_type == TYPE_BOOL || stack->items.var_type == TYPE_UNDEFINED)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            if(stack->items.var_type == TYPE_STRING || stack->next->next->items.var_type == TYPE_STRING)
                {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

            if(stack->items.var_type == TYPE_INT){
                if(stack->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }
            
            if(stack->next->next->items.var_type == TYPE_INT){
                if(stack->next->next->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }
        }

        new_expression->can_be_nil = false;
        new_expression->var_type = TYPE_BOOL;
        break;
    
    case LESS_T:
        valid = rule3(stack, rule);
        if(stack->items.var_type == TYPE_BOOL || stack->items.var_type == TYPE_UNDEFINED || stack->items.var_type == TYPE_NIL)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            if(stack->items.var_type == TYPE_STRING || stack->next->next->items.var_type == TYPE_STRING)
                {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

            if(stack->items.var_type == TYPE_INT){
                if(stack->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }
            
            if(stack->next->next->items.var_type == TYPE_INT){
                if(stack->next->next->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }
        }

        if(stack->items.can_be_nil == true || stack->next->next->items.can_be_nil == true)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        new_expression->can_be_nil = false;
        new_expression->var_type = TYPE_BOOL;
        break;

    case GREATER_T:
        valid = rule3(stack, rule);
        if(stack->items.var_type == TYPE_BOOL || stack->items.var_type == TYPE_UNDEFINED || stack->items.var_type == TYPE_NIL)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
            
        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            if(stack->items.var_type == TYPE_STRING || stack->next->next->items.var_type == TYPE_STRING)
                {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

            if(stack->items.var_type == TYPE_INT){
                if(stack->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }
            
            if(stack->next->next->items.var_type == TYPE_INT){
                if(stack->next->next->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }
        }

        if(stack->items.can_be_nil == true || stack->next->next->items.can_be_nil == true)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        new_expression->can_be_nil = false;
        new_expression->var_type = TYPE_BOOL;
        break;

    case LESS_EQUAL_T:
        valid = rule3(stack, rule);
        if(stack->items.var_type == TYPE_BOOL || stack->items.var_type == TYPE_UNDEFINED || stack->items.var_type == TYPE_NIL)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            if(stack->items.var_type == TYPE_STRING || stack->next->next->items.var_type == TYPE_STRING)
                {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

            if(stack->items.var_type == TYPE_INT){
                if(stack->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }
            
            if(stack->next->next->items.var_type == TYPE_INT){
                if(stack->next->next->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }
        }

        if(stack->items.can_be_nil == true || stack->next->next->items.can_be_nil == true)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        new_expression->can_be_nil = false;
        new_expression->var_type = TYPE_BOOL;
        break;

    case GREATER_EQUAL_T:
        valid = rule3(stack, rule);
        if(stack->items.var_type == TYPE_BOOL || stack->items.var_type == TYPE_UNDEFINED || stack->items.var_type == TYPE_NIL)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.var_type != stack->next->next->items.var_type)
        {
            if(stack->items.var_type == TYPE_STRING || stack->next->next->items.var_type == TYPE_STRING)
                {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

            if(stack->items.var_type == TYPE_INT){
                if(stack->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }
            
            if(stack->next->next->items.var_type == TYPE_INT){
                if(stack->next->next->items.is_lit == false)
                    {ERROR_HANDLE_PREC(TYPE_ERROR, token);}
                // int2double TODO
            }
        }

        if(stack->items.can_be_nil == true || stack->next->next->items.can_be_nil == true)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        new_expression->can_be_nil = false;
        new_expression->var_type = TYPE_BOOL;
        break;
    
    case DOUBLE_QUESTION_MARK_T:
        valid = rule3(stack, rule);
        if(stack->items.var_type != stack->next->next->items.var_type && stack->next->next->items.type == TYPE_NIL)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        if(stack->items.can_be_nil == true)
            {ERROR_HANDLE_PREC(TYPE_ERROR, token);}

        new_expression->can_be_nil = false;
        new_expression->var_type = stack->items.var_type;
        break;
    
    case EXCLAMATION_POINT_T:
        valid = (check_stoppage(stack, 2) && stack->items.type == rule.type && stack->next->items.type == EXPRESSION_T);
        new_expression->can_be_nil = false;
        new_expression->var_type = stack->next->items.var_type;
        new_expression->is_lit = stack->next->items.is_lit;
        break;
    
    case RIGHT_PAR_T:
        valid = (check_stoppage(stack,3) && stack->items.type == RIGHT_PAR_T && stack->next->items.type == EXPRESSION_T && stack->next->next->items.type == LEFT_PAR_T);
        new_expression->can_be_nil = stack->next->items.can_be_nil;
        new_expression->var_type = stack->next->items.var_type;
        new_expression->is_lit = stack->next->items.is_lit;
        break;
    
    case TERM_T:
        valid = rule1(stack, rule);
        new_expression->can_be_nil = stack->items.can_be_nil;
        new_expression->var_type = stack->items.var_type;
        new_expression->is_lit = stack->items.is_lit;
        break;  
    
    default:
        ERROR_HANDLE_PREC(SYNTAX_ERROR, token);
        break;
    }
    return valid;
}

data_type_t precedent_analysys(Lexeme *lexeme, symtable_stack_t *sym_stack)
{
    bool valid = true;
    bool cont = true;
    prec_stack_t *stack;
    stack_init(&stack);
    stack_rules_t stack_rule;
    symtable_item_t *variable;
    valid_itmes_t new_expression;

    if(lexeme->kind == IDENTIFIER)
    {
        variable = SymtableSearchAll(sym_stack, lexeme->extra_data.string);
        if(variable == NULL)
            {ERROR_HANDLE_PREC(UNDEFINED_VAR_ERROR, lexeme);}
    }

    valid_itmes_t input = convert_lex_term(*lexeme, sym_stack);
    
    while(valid)
    {
        //check if variable was defined
        if(lexeme->kind == IDENTIFIER)
        {
            variable = SymtableSearchAll(sym_stack, lexeme->extra_data.string);
            if(variable == NULL)
                {ERROR_HANDLE_PREC(UNDEFINED_VAR_ERROR, lexeme);}
        }

        stack_rule = give_stack_rule(stack, input.type);

        switch (stack_rule)
        {
        case SHIFT_R:
            stack_push(&stack, &input);
            *lexeme = get_next_non_whitespace_lexeme();
            input = convert_lex_term(*lexeme, sym_stack);
            break;
        case STOPPAGE_R:
            stack_push_stoppage(&stack);
            stack_push(&stack, &input);
            *lexeme = get_next_non_whitespace_lexeme();
            input = convert_lex_term(*lexeme, sym_stack);
            break;
        case MERGE_R:
            if(check_prec_rule(stack, &new_expression, lexeme))
                stack_merge(&stack, new_expression);
            else
                {ERROR_HANDLE_PREC(SYNTAX_ERROR, lexeme);}
            break;
        case ERROR_R:
            valid = false;
            break;
        default:
            ERROR_HANDLE_PREC(SYNTAX_ERROR, lexeme);
            break;
        }
    }

    // second traverse with input = $
    input.type = DOLLAR_T;
    while(cont == true)
    {

        // chceck if variable was defined
        if(lexeme->kind == IDENTIFIER)
        {
            variable = SymtableSearchAll(sym_stack, lexeme->extra_data.string);
            if(variable == NULL)
                {ERROR_HANDLE_PREC(UNDEFINED_VAR_ERROR, lexeme);}
        }
        
        stack_rule = give_stack_rule(stack, input.type);
        if(stack_rule == MERGE_R){
            if(check_prec_rule(stack, &new_expression, lexeme))
                stack_merge(&stack, new_expression);
            else
                {ERROR_HANDLE_PREC(SYNTAX_ERROR, lexeme);}
        }
        else
            cont = false;
    }
    
    // check if all syntax operations valid
        if(!stack_empty(stack))
        {ERROR_HANDLE_PREC(SYNTAX_ERROR, lexeme);}

    data_type_t exit_data_type = stack->items.var_type;
    stack_dispose(&stack); 

    return exit_data_type;
}
