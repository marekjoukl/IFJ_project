/**
 * @file generator.c
 * @author Marek Joukl (xjoukl00)
 * @brief Code generator
 */

#include "generator.h"
#include "builtin_func.h"

void StartGenerator(Generator *g){
    str_init(&g->header);
    str_init(&g->instructions);
    str_init(&g->footer);
    str_init(&g->stack_values);
    print_header(g);
    print_footer(g);
}

/************** FUNCTIONS FOR DYNAMIC ARRAY **************/

void str_init(String *s){
    s->str = malloc(INIT_STRING_SIZE);
    if (s->str == NULL)
    {
        fprintf(stderr, "Error: generator.c - str_init() - malloc failed\n");
        exit(99);
    }
    s->str[0] = '\0';
    s->length = 0;
    s->alloc_size = INIT_STRING_SIZE;
}

void add_to_str(String *s, char *str){
    int len = strlen(str);
    if(s->length + len + 1 >= s->alloc_size){
        s->alloc_size += len + 1;
        char *tmp = realloc(s->str, s->alloc_size);
        if (tmp == NULL)
        {
            fprintf(stderr, "Error: generator.c - add_to_string() - realloc failed\n");
            exit(99);
        }
        s->str = tmp;
    }
    s->length += len;
    strcat(s->str, str);
}

void add_chr_to_str(String *s, char c){
    if(s->length + 1 >= s->alloc_size){
        s->alloc_size *= 2;
        char *tmp = realloc(s->str, s->alloc_size);
        if (tmp == NULL)
        {
            fprintf(stderr, "Error: generator.c - add_chr_to_string() - realloc failed\n");
            exit(99);
        }
        s->str = tmp;
    }
    s->str[s->length] = c;
    s->length++;
    s->str[s->length] = '\0';
}

void remove_ch_from_str(String *s){
    memmove(&s->str[0], &s->str[1], s->length - 1);
    s->str[s->length - 1] = '\0';
    s->length--;
}

void str_dtor(String *s){
    free(s->str);
    s->alloc_size = 0;
    s->length = 0;
    s->str = NULL;
}

void str_clear(String *s){
    s->str[0] = '\0';
    s->length = 0;
}

void cleanup_generator(Generator *g){
    str_dtor(&g->header);
    str_dtor(&g->instructions);
    str_dtor(&g->footer);
    str_dtor(&g->stack_values);
}

/************** FUNCTIONS FOR GENERATOR **************/

void print_header(Generator *g){
    add_to_str(&g->header, ".IFJcode23\n");
    // VARIABLES
    add_to_str(&g->header, "DEFVAR GF@!TYPE1\n");
    add_to_str(&g->header, "DEFVAR GF@!TYPE2\n");
    add_to_str(&g->header, "DEFVAR GF@!var1\n");
    add_to_str(&g->header, "DEFVAR GF@!var2\n");
    add_to_str(&g->header, "DEFVAR GF@!var3\n");
    add_to_str(&g->header, "JUMP $main\n");

    // FUNCTIONS
    add_to_str(&g->header, "\n");
    builtin_write(g);
    add_to_str(&g->header, "\n");
    builtin_readInt(g);
    add_to_str(&g->header, "\n");
    builtin_readString(g);
    add_to_str(&g->header, "\n");
    builtin_readDouble(g);
    add_to_str(&g->header, "\n");
    builtin_length(g);
    add_to_str(&g->header, "\n");
    builtin_chr(g);
    add_to_str(&g->header, "\n");
    builtin_ord(g);
    add_to_str(&g->header, "\n");
    eval_bool(g);
    add_to_str(&g->header, "\n");
    eval_equals(g);
    add_to_str(&g->header, "\n");
    eval_greater(g);
    add_to_str(&g->header, "\n");
    eval_greater_equal(g);
    add_to_str(&g->header, "\n");
    add_to_str(&g->header, "LABEL $main\n");
}

void print_footer(Generator *g){  
    add_to_str(&g->header, "\n");  
    add_to_str(&g->footer,  "EXIT int@0\n"                      // no errors
                            "LABEL !ERROR!FUNC!NOT!DEF\n"       // func not defined or redifinition of variable
                            "EXIT int@3\n"
                            "LABEL !ERROR!CALL!FUNC\n"          // wrong parameters while calling or wrong return type
                            "EXIT int@4\n");
}

void if_stat(Generator *g){
    add_to_str(&g->instructions, "CALL $eval_bool\n"
                                 "JUMPIFEQ $else GF@!tmp1 bool@false\n");       // if false, jump to else
}

void else_stat(Generator *g){
    add_to_str(&g->instructions, "LABEL $else\n");
}

void while_loop_gen(Generator *g){
    add_to_str(&g->instructions, "LABEL $while\n"
                                 "CALL $eval_bool\n"
                                 "JUMPIFEQ $end_while GF@!tmp1 bool@false\n");       // if false, jump to end_while
}

void while_loop_end(Generator *g){
    add_to_str(&g->instructions, "JUMP $while\n"
                                 "LABEL $end_while\n");
}

void call_builtin_write(Generator *g){
    // TODO
}

void extract_value(Generator *g, Lexeme *token){
    switch (token->kind)
    {
    case INTEGER_LIT:{
        add_to_str(&g->instructions, "PUSHS int@");
        char buffer[16];
        sprintf(buffer, "%d", token->extra_data.IntValue);
        add_to_str(&g->stack_values, buffer);
        break;
        }
    case DOUBLE_LIT:{
        add_to_str(&g->instructions, "PUSHS float@");
        char buffer[32];
        sprintf(buffer, "%f", token->extra_data.DoubleValue);
        add_to_str(&g->stack_values, buffer);
        break;
        }
    case STRING_LIT:{
        // TODO
        add_to_str(&g->instructions, "PUSHS string@");
        }
        break;
    case IDENTIFIER:
        // TODO
        //add_to_str(&g->instructions, "PUSHS LF@");
        //add_to_str(&g->instructions, token->extra_data.IntValue);
        add_to_str(&g->instructions, "\n");
        break;
    case NIL:{
        add_to_str(&g->instructions, "PUSHS nil@nil\n");
        break;
        }
    default:
        fprintf(stderr, "Error: generator.c - extract_value() - unknown token type\n");
        exit(99);
        break;
    }
}

void exp_postfix(Generator *g, Lexeme *token){

    // IF EXPRESSION IS JUST A NUMBER, INSERT IT TO STACK, ELSE DO OPERATIONS

    switch (token->kind)
    {
        case PLUS:
            add_to_str(&g->instructions, "ADDS\n");
            break;
        case MINUS:
            add_to_str(&g->instructions, "SUBS\n");
            break;
        case ASTERISK:
            add_to_str(&g->instructions, "MULS\n");
            break;
        case SLASH:
            add_to_str(&g->instructions, "DIVS\n");
            break;
        case LESS:
            add_to_str(&g->instructions, "CALL $eval_greater_equal\n");  // NEGATE
            break;
        case GREATER:
            add_to_str(&g->instructions, "CALL $eval_greater\n");
            break;
        case LESS_EQUAL:
            add_to_str(&g->instructions, "CALL $eval_greater\n");        // NEGATE
            break;
        case GREATER_EQUAL:
            add_to_str(&g->instructions, "CALL $eval_greater_equal\n");
            break;
        case EQUAL:
            add_to_str(&g->instructions, "CALL $eval_equals\n");
            break;
        case NOT_EQUAL:
            add_to_str(&g->instructions, "CALL $eval_equals\n");
            break;
        default:
            fprintf(stderr, "Error: generator.c - exp_postfix() - unknown token type\n");
            exit(99);
            break;
    }
}

void function_gen(Generator *g, Lexeme *token){
    add_to_str(&g->instructions, "DEFVAR GF@");
    add_to_str(&g->instructions, token->extra_data.string);
    // TODO
}



