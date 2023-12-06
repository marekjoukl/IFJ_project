/** 
 * Implementace překladače imperativního jazyka IFJ23
 * xputis00 Daniel Puts
 * xzelen29  Jakub Zelenay
 * xjoukl00 Marek Joukul
 * xkozan01 Ondrej Kožanyi
*/

#include "precedent.h"

// ====================================================
//                  syntax_stack
// ====================================================

void stack_init(prec_stack_t **stack)
{
    *stack = NULL;
    valid_itmes_t first;
    first.type = DOLLAR_T;
    first.posfix_name = NULL;
    first.tree = NULL;
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

//================ astree =====================

void tree_init(ast_t **tree)
{
    *tree = NULL;
}

void tree_link(ast_t **parent, ast_t *left, ast_t *right)
{
    if(*parent == NULL || (*parent)->left != NULL || (*parent)->right != NULL){
        fprintf(stderr, "error tree link\n"); //debug
        exit(INTERNAL_ERROR);
    }

    (*parent)->left = left;
    (*parent)->right = right;

}

void tree_insert(ast_t **tree, char *data)
{
    ast_t *new = malloc(sizeof(ast_t));
    if(new == NULL)
        {ERROR_HANDLE_PREC_STACK(INTERNAL_ERROR);}
    
    new->data = malloc(sizeof(char) *(strlen(data) + 1));
    new-> data = data;
    new->left = *tree;
    new->right = NULL;

    *tree = new;
}

void tree_postorder(ast_t *tree)
{   
    if(tree == NULL)
        return;
    tree_postorder(tree->left);
    tree_postorder(tree->right);

    printf("%s,", tree->data);
}

void tree_dispose(ast_t **tree)
{
    if(*tree == NULL)
        return;
    tree_dispose(&((*tree)->left));
    tree_dispose(&((*tree)->right));

    // free((*tree)->data);
    (*tree)->data = NULL;
    free(*tree);
    *tree = NULL;
    tree = NULL;
}
