/**
 * @file generator.c
 * @author Marek Joukl (xjoukl00)
 * @brief Code generator
 */

#include "generator.h"
#include "builtin_func.h"

Generator g;

void StartGenerator(Generator *g){
    str_init(&g->header);
    str_init(&g->instructions);
    str_init(&g->footer);
}

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

void cleanup(Generator *g){
    str_dtor(&g->header);
    str_dtor(&g->instructions);
    str_dtor(&g->footer);
}

void print_header(Generator *g){
    add_to_str(&g->header, ".IFJcode23\n");
    add_to_str(&g->header, "JUMP $main\n");
    // VARIABLES
    add_to_str(&g->header, "DEFVAR GF@!TYPE1\n");
    add_to_str(&g->header, "DEFVAR GF@!TYPE2\n");
    add_to_str(&g->header, "DEFVAR GF@!var1\n");
    add_to_str(&g->header, "DEFVAR GF@!var2\n");
    add_to_str(&g->header, "DEFVAR GF@!var3\n");

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
    add_to_str(&g->header, "LABEL $main\n");
}

void print_footer(Generator *g){  
    add_to_str(&g->header, "\n");  
    add_to_str(&g->footer,  "EXIT int@0\n"                      // no errors
                            "LABEL !ERROR!FUNC!NOT!DEF\n"       // func not defined or redifinition of variable
                            "EXIT int@3\n"
                            "LABEL !ERROR!CALL!FUNC\n"          // wrong parameters while calling or wrong return type
                            "EXIT int@4\n"
                            "LABEL !ERROR!WRONG!VAR!USAGE\n"    //using not defined or not initialized variable
                            "EXIT int@5\n"
                            "LABEL !ERROR!RETURN\n"             // wrong return expression
                            "EXIT int@6\n"
                            "LABEL !ERROR!TYPE\n"
                            "EXIT int@7\n");
}

void if_stat(Generator *g){
    add_to_str(&g->instructions, "CALL $eval_bool\n"
                                 "JUMPIFEQ $else GF@!tmp1 bool@false\n");       // if false, jump to else

}

void else_stat(Generator *g){
    add_to_str(&g->instructions, "LABEL $else\n");
}


