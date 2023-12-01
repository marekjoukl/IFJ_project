#include "precedent.h"
//#include "precedent_stack.h"

void stack_init(prec_stack_t **stack)
{
    *stack = NULL;
    valid_itmes_t first;
    first.type = DOLLAR_T;
    stack_push(stack, &first);
}

bool stack_empty(prec_stack_t *stack)
{
    valid_itmes_t destination;
    stack_top_terminal(stack, &destination);
    return (stack != NULL && destination.type == DOLLAR_T );
}

void stack_push(prec_stack_t **stack, valid_itmes_t *item) //mozno by mohol byt typ lexeme pre item?
{
    prec_stack_t *new = malloc(sizeof(prec_stack_t));
    if(new == NULL)
        {ERROR_HANDLE_PREC_STACK(INTERNAL_ERROR);}
    new->items = *item;
    new->next = *stack;
    *stack = new;
}

void stack_top_terminal(prec_stack_t *stack, valid_itmes_t *destination)
{   
    while(stack != NULL)
    {
        if(stack->items.type != EXPRESSION_T)
        {
            *destination = (stack->items);
            return;
        }
        else
        stack = stack->next;
    }
}

void stack_push_stoppage(prec_stack_t **stack)
{
    prec_stack_t *tmp = *stack;
    while(tmp != NULL)
    {
        if(tmp->items.type != EXPRESSION_T)
        {
            prec_stack_t *new = malloc(sizeof(prec_stack_t));
            if(new == NULL) 
                {ERROR_HANDLE_PREC_STACK(INTERNAL_ERROR);}

            new->items = tmp->items;
            new->next = tmp->next;
            tmp->items.type = STOPPAGE_T;
            new->next = tmp->next;
            tmp->next = new;
            return;
        }
        tmp = tmp->next;
    }
}

void stack_merge(prec_stack_t **stack)
{
    prec_stack_t *tmp;
    while(*stack != NULL && (*stack)->items.type != STOPPAGE_T)
    {
        tmp = *stack;
        *stack = (*stack)->next;
        free(tmp);
    }
    if(*stack != NULL && (*stack)->items.type == STOPPAGE_T)
    {
        tmp = *stack;
        *stack = (*stack)->next;
        free(tmp);
    }

    valid_itmes_t expression = {EXPRESSION_T};
    stack_push(stack, &expression);
}

bool check_stoppage(prec_stack_t *stack, int cnt)
{
    bool valid = true;
    while (valid == true && cnt > 0)
    {
        if(stack == NULL || stack->items.type == STOPPAGE_T)
            valid = false;
        cnt--;
    }
    return valid;
}

bool rule1(prec_stack_t *stack, valid_itmes_t rule)
{
    return (check_stoppage(stack,2) && stack->items.type == rule.type);
}

bool rule3(prec_stack_t *stack, valid_itmes_t rule)
{
    return (check_stoppage(stack,3) && stack->items.type == EXPRESSION_T && stack->next->items.type == rule.type && stack->next->next->items.type == EXPRESSION_T);
}

bool check_prec_rule(prec_stack_t *stack)
{
    valid_itmes_t rule;
    stack_top_terminal(stack, &rule);
    bool valid = false;
    switch (rule.type)
    {
    case MUL_T:
        valid = rule3(stack, rule);    
        break;
    case DIV_T:
        valid = rule3(stack, rule);    
        break;
    case PLUS_T:
        valid = rule3(stack, rule);    
        break;
    case MINUS_T:
        valid = rule3(stack, rule);    
        break;
    case EQUAL_T:
        valid = rule3(stack, rule);    
        break;
    case NOT_EQUAL_T:
        valid = rule3(stack, rule);    
        break;
    case LESS_T:
        valid = rule3(stack, rule);    
        break;
    case GREATER_T:
        valid = rule3(stack, rule);    
        break;
    case LESS_EQUAL_T:
        valid = rule3(stack, rule);    
        break;
    case GREATER_EQUAL_T:
        valid = rule3(stack, rule);    
        break;
    case DOUBLE_QUESTION_MARK_T:
        valid = rule3(stack, rule);    
        break;
    case EXCLAMATION_POINT_T:
        valid = (check_stoppage(stack, 2) && stack->items.type == rule.type && stack->next->items.type == EXPRESSION_T);
        break;
    case RIGHT_PAR_T:
        valid = (check_stoppage(stack,3) && stack->items.type == RIGHT_PAR_T && stack->next->items.type == EXPRESSION_T && stack->next->next->items.type == LEFT_PAR_T);
        break;
    case TERM_T:
        valid = rule1(stack, rule);
        break;  
    default:
        valid = false;
        break;
    }
    return valid;
}

void stack_dispose(prec_stack_t **stack)
{
    prec_stack_t *tmp;
    while(*stack != NULL)
    {
        tmp = *stack;
        *stack = (*stack)->next;
        free(tmp);
    }
    *stack = NULL;
}
