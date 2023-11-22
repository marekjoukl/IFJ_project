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
        return;
    new->items.type = item->type;
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
