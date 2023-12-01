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


// #define max_test2 3
// valid_itmes_t test_items2[] = {{EXPRESSION_T}, {MUL_T}, {EXPRESSION_T}};

// TEST(test_init, "init")
// stack_init(&test_stack);
// print_stack(test_stack);
// printf("%d\n", stack_empty(test_stack));
// stack_dispose(&test_stack);
// ENDTEST

// TEST(push_items, "push more items")
// stack_init(&test_stack);
// for(int i = 0; i< max_test2; i++)
//     stack_push(&test_stack, &(test_items2[i]));
// print_stack(test_stack);
// stack_dispose(&test_stack);
// ENDTEST

// TEST(push_stoppage, "symulate \"<\" rule")
// stack_init(&test_stack);
// for(int i = 0; i< max_test2; i++)
//     stack_push(&test_stack, &(test_items2[i]));
// stack_push_stoppage(&test_stack);
// print_stack(test_stack);
// stack_dispose(&test_stack);
// ENDTEST

// TEST(merge, "simulate \">\" rule")
// stack_init(&test_stack);
// stack_push(&test_stack, &(test_items2[0]));
// stack_push_stoppage(&test_stack);
// for(int i = 1; i< max_test2; i++)
//     stack_push(&test_stack, &(test_items2[i]));
// print_stack(test_stack);
// stack_merge(&test_stack);
// print_stack(test_stack);
// stack_dispose(&test_stack);
// ENDTEST

TEST(rule, "test give rule")
stack_init(&test_stack);
printf("rule = %d\n" ,give_stack_rule(test_stack, LEFT_PAR_T));
ENDTEST

TEST(basic, "test basic precedent and seamnatic without variables")
Lexeme token = get_next_non_whitespace_lexeme();
symtable_stack_t *stack = SymtableStackInit();
Symtable *table = malloc(sizeof(Symtable));
SymtableInit(table);
SymtableStackPush(stack, table);

bool valid = precedent_analysys(&token, stack);
printf("valid basic test = %d, next token = %d", valid, token.kind);

SymtableStackPop(stack);
SymtableStackDispose(stack);
ENDTEST

TEST(advanced, "test with premade variables")
Lexeme token = get_next_non_whitespace_lexeme();
symtable_stack_t *stack = SymtableStackInit();
Symtable *table = malloc(sizeof(Symtable));
SymtableInit(table);
SymtableStackPush(stack, table);
data_t testdata[] = {{TYPE_INT, 0, 0, 0, NULL, NULL, NULL, 0, 0, NULL, 0.0, 0, 0}, {TYPE_DOUBLE, 0, 0, 0, NULL, NULL, NULL, 0, 0, NULL, 0.0, 0, 0}, {TYPE_STRING, 0, 0, 0, NULL, NULL, NULL, 0, 0, NULL, 0.0, 0, 0}};
SymtableAddItem(table, "a", &(testdata[0]));

bool valid = precedent_analysys(&token, stack);
printf("valid advanced test = %d, next token = %d", valid, token.kind);

SymtableStackPop(stack);
SymtableStackDispose(stack);
ENDTEST

int main(void)
{
    // test_init();
    // push_items();
    // push_stoppage();
    // merge();
    // rule();
    basic();
    // advanced();
}
