#include "precedent.h"

// ====================================================
//                  syntax_stack
// ====================================================

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
        {ERROR_HANDLE_PREC_STACK(INTERNAL_ERROR);}
    new->items = *item;
    new->next = *stack;
    *stack = new;
}

bool stack_top(prec_stack_t *stack, valid_itmes_t *dst)
{   
    if(stack == NULL || stack->items.type == DOLLAR_T)
        return false;
    *dst = (stack->items);
    return true;
}

void stack_top_terminal(prec_stack_t *stack, valid_itmes_t *dst)
{   
    while(stack != NULL)
    {
        if(stack->items.type != EXPRESSION_T)
        {
            *dst = (stack->items);
            return;
        }
        else
        stack = stack->next;
    }
}

void stack_pop(prec_stack_t **stack)
{
    prec_stack_t *tmp = *stack;
    *stack = (*stack)->next;
    free(tmp);
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

// ====================================================
//              postix_notation
// ====================================================
void add_postfix(postix_array_t *postfix ,char * name)
{
    int  new_size = strlen(name);

    if(postfix->size + new_size >= postfix->capacity){
        postfix->capacity += postfix->capacity + new_size + POSTFIX_CHUNK;
        postfix->array = realloc(postfix->array, sizeof(char*) * postfix->capacity);
    }

    strcat(postfix->array, name);
    strcat(postfix->array, ",");

    (postfix->size)+= new_size;
}

void front_init(prec_stack_t **front)
{
    *front = NULL;
}

void front_front(prec_stack_t **front, valid_itmes_t *item)
{
    prec_stack_t *tmp = *front;
    prec_stack_t *new = malloc(sizeof(prec_stack_t));
    if(new == NULL)
        {ERROR_HANDLE_PREC_STACK(INTERNAL_ERROR);}
    new->items = *item;
    new->next = NULL;

    if(*front == NULL)
        *front = new;
    else
    {
        while(tmp->next != NULL)
        {
            tmp = tmp->next;
        }
        tmp->next = new;
    }
}

bool front_top(prec_stack_t *front, valid_itmes_t *dst)
{
    if(front == NULL)
        return false;
    *dst = front->items;
    return true;
}

void front_pop(prec_stack_t **front)
{
    prec_stack_t *tmp = *front;
    *front = (*front)->next;
    free(tmp);
}

bool front_one(prec_stack_t *front)
{
    return (front != NULL && front->next == NULL);
}

// void front_dipose(){}