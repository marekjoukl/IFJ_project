#include "precedent.h"
//#include "precedent_stack.h"

#define TEST(NAME, DESCRIPTION)                                                \
  void NAME() {                                                                \
    printf("[%s] %s\n", #NAME, DESCRIPTION);                                   \
    prec_stack_t *test_stack;                                                  \

#define ENDTEST                                                                \
  printf("\n");                                                                \
  stack_dispose(&test_stack);                                                  \
  }


void print_stack(prec_stack_t *stack)
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


#define max_test2 3
valid_itmes_t test_items2[] = {{EXPRESSION_T}, {MUL_T}, {EXPRESSION_T}};

TEST(test_init, "init")
stack_init(&test_stack);
print_stack(test_stack);
stack_dispose(&test_stack);
ENDTEST

TEST(push_items, "push more items")
stack_init(&test_stack);
for(int i = 0; i< max_test2; i++)
    stack_push(&test_stack, &(test_items2[i]));
print_stack(test_stack);
stack_dispose(&test_stack);
ENDTEST

TEST(push_stoppage, "symulate \"<\" rule")
stack_init(&test_stack);
for(int i = 0; i< max_test2; i++)
    stack_push(&test_stack, &(test_items2[i]));
stack_push_stoppage(&test_stack);
print_stack(test_stack);
stack_dispose(&test_stack);
ENDTEST

TEST(merge, "simulate \">\" rule")
stack_init(&test_stack);
stack_push(&test_stack, &(test_items2[0]));
stack_push_stoppage(&test_stack);
for(int i = 1; i< max_test2; i++)
    stack_push(&test_stack, &(test_items2[i]));
print_stack(test_stack);
stack_merge(&test_stack);
print_stack(test_stack);
stack_dispose(&test_stack);
ENDTEST



int main()
{
    test_init();
    push_items();
    push_stoppage();
    merge();
}