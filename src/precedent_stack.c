#include "precedent.h"

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

void stack_push(prec_stack_t **stack, valid_itmes_t *item)
{
    prec_stack_t *new = malloc(sizeof(prec_stack_t));
    if(new == NULL)
        return;
    new->items.type = item->type;
    new->items.can_be_nil = item->can_be_nil;
    new->items.var_type = item->var_type;
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
                return;
            new->items.type = tmp->items.type;
            new->items.can_be_nil = tmp->items.can_be_nil;
            new->items.var_type = tmp->items.var_type;
            new->next = tmp->next;
            tmp->items.type = STOPPAGE_T;
            tmp->next = new;
            return;
        }
        tmp = tmp->next;
    }
}

void stack_merge(prec_stack_t **stack, valid_itmes_t new_expression)
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

    stack_push(stack, &new_expression);
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
    return (check_stoppage(stack,3) && 
            stack->items.type == EXPRESSION_T &&
            stack->next->items.type == rule.type && 
            stack->next->next->items.type == EXPRESSION_T);
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
