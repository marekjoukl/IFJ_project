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
    str_init(&g->vars);
    str_init(&g->function_call_tmps);
    str_init(&g->temp_string);
    g->parameters_count = 0;
    //str_init(&g->parameters);
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
    str_dtor(&g->vars);
    //str_dtor(&g->parameters);
}

/************** FUNCTIONS FOR GENERATOR **************/

void print_header(Generator *g){
    add_to_str(&g->header, ".IFJcode23\n");
    // VARIABLES
    add_to_str(&g->header, "DEFVAR GF@!TYPE1\n");
    add_to_str(&g->header, "DEFVAR GF@!TYPE2\n");
    add_to_str(&g->header, "DEFVAR GF@!tmp1\n");
    add_to_str(&g->header, "DEFVAR GF@!tmp2\n");
    add_to_str(&g->header, "DEFVAR GF@!var3\n");
    add_to_str(&g->header, "DEFVAR GF@!stack_var\n");
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
    eval_less(g);
    add_to_str(&g->header, "\n");
    eval_less_equal(g);
    add_to_str(&g->header, "\n");
    eval_not_equals(g);
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

void if_stat(Generator *g, ast_t *asttree, symtable_stack_t *stack, int if_counter){
    exp_postfix(g, asttree, stack);
    add_to_str(&g->instructions, "PUSHS bool@true\n"
                                 "JUMPIFNEQS $else");       // if false, jump to else
    char buffer[11];
    sprintf(buffer, "%d", if_counter);
    add_to_str(&g->instructions, buffer);
    add_to_str(&g->instructions, "\n");
    add_to_str(&g->instructions, "CREATEFRAME\n");
    add_to_str(&g->instructions, "PUSHFRAME\n");
}

void else_stat(Generator *g, bool is_else_start, int else_counter){
    char buffer[11];
    sprintf(buffer, "%d", else_counter);
    if (is_else_start) {
        add_to_str(&g->instructions, "POPFRAME\n");
        add_to_str(&g->instructions, "JUMP $else_end");
        add_to_str(&g->instructions, buffer);
        add_to_str(&g->instructions, "\n");
        add_to_str(&g->instructions, "LABEL $else");
        add_to_str(&g->instructions, buffer);
        add_to_str(&g->instructions, "\n");
        add_to_str(&g->instructions, "CREATEFRAME\n");
        add_to_str(&g->instructions, "PUSHFRAME\n");
    }
    else {
        add_to_str(&g->instructions, "POPFRAME\n");
        add_to_str(&g->instructions, "LABEL $else_end");
        add_to_str(&g->instructions, buffer);
        add_to_str(&g->instructions, "\n");
    }
}

void while_loop_gen(Generator *g, symtable_stack_t *stack, ast_t *asttree, int while_counter){
    char buffer[11];
    sprintf(buffer, "%d", while_counter);
    add_to_str(&g->instructions, "CREATEFRAME\n");
    add_to_str(&g->instructions, "PUSHFRAME\n");
    add_to_str(&g->instructions, "LABEL $while");
    add_to_str(&g->instructions, buffer);
    add_to_str(&g->instructions, "\n");
    exp_postfix(g, asttree, stack);
    add_to_str(&g->instructions, "PUSHS bool@true\n");
    add_to_str(&g->instructions, "JUMPIFNEQS $end_while");
    add_to_str(&g->instructions, buffer);
    add_to_str(&g->instructions, "\n");
}

void while_loop_end(Generator *g, int while_counter){
    char buffer[11];
    sprintf(buffer, "%d", while_counter);
    add_to_str(&g->instructions, "JUMP $while");
    add_to_str(&g->instructions, buffer);
    add_to_str(&g->instructions, "\n");
    add_to_str(&g->instructions, "LABEL $end_while");
    add_to_str(&g->instructions, buffer);
    add_to_str(&g->instructions, "\n");
    add_to_str(&g->instructions, "POPFRAME\n");
}

void extract_value(Generator *g, Lexeme *token, symtable_item_t *item, symtable_stack_t *stack){
    switch (token->kind)
    {
    case INTEGER_LIT:{
        add_to_str(&g->temp_string, "PUSHS int@");
        add_to_str(&g->temp_string, token->extra_data.string);
        add_to_str(&g->temp_string, "\n");
        g->parameters[g->parameters_count-1] = malloc(sizeof(char) * (g->temp_string.length+1));
        strcpy(g->parameters[g->parameters_count-1], g->temp_string.str);
        str_clear(&g->temp_string);
        break;
        }
    case DOUBLE_LIT:{
        add_to_str(&g->temp_string, "PUSHS float@");
        add_to_str(&g->temp_string, token->extra_data.string);
        add_to_str(&g->temp_string, "\n");
        g->parameters[g->parameters_count-1] = malloc(sizeof(char) * (g->temp_string.length+1));
        strcpy(g->parameters[g->parameters_count-1], g->temp_string.str);
        str_clear(&g->temp_string);
        break;
        }
    case STRING_LIT:{
        add_to_str(&g->temp_string, "PUSHS string@");
        add_to_str(&g->temp_string, token->extra_data.string);
        add_to_str(&g->temp_string, "\n");
        g->parameters[g->parameters_count-1] = malloc(sizeof(char) * (g->temp_string.length+1));
        strcpy(g->parameters[g->parameters_count-1], g->temp_string.str);
        str_clear(&g->temp_string);
        }
        break;
    case IDENTIFIER:
        if (stack->size == 1) {
            // We are in global frame
            add_to_str(&g->temp_string, "PUSHS GF@");
        } else {
            add_to_str(&g->temp_string, "PUSHS LF@");
        }
        add_to_str(&g->temp_string, token->extra_data.string);
        add_to_str(&g->temp_string, "\n");
        g->parameters[g->parameters_count-1] = malloc(sizeof(char) * (g->temp_string.length+1));
        strcpy(g->parameters[g->parameters_count-1], g->temp_string.str);
        str_clear(&g->temp_string);
        break;
    case NIL:{
        add_to_str(&g->temp_string, "PUSHS nil@nil\n");
        g->parameters[g->parameters_count-1] = malloc(sizeof(char) * (g->temp_string.length+1));
        strcpy(g->parameters[g->parameters_count-1], g->temp_string.str);
        str_clear(&g->temp_string);
        break;
        }
    default:
        fprintf(stderr, "Error: generator.c - extract_value() - unknown token type\n");
        exit(99);
        break;
    }
}

void exp_postfix(Generator *g, ast_t *tree, symtable_stack_t *stack){
    if(tree == NULL){
        return;
    }
    exp_postfix(g, tree->left, stack);
    exp_postfix(g, tree->right, stack);

    if (strcmp(tree->data, "+") == 0){
        add_to_str(&g->instructions, "ADDS\n");
    } else if (strcmp(tree->data, "+") == 0){
        add_to_str(&g->instructions, "CALL $concat_prep\n");    // TODO
    }
    else if (strcmp(tree->data, "-") == 0)
    {
        add_to_str(&g->instructions, "SUBS\n");
    } else if (strcmp(tree->data, "*") == 0)
    {
        add_to_str(&g->instructions, "MULS\n");
    } else if (strcmp(tree->data, "/") == 0)
    {
        if (tree->right->type == TYPE_INT)
            add_to_str(&g->instructions, "IDIVS\n");
        else
            add_to_str(&g->instructions, "DIVS\n");
    } else if (strcmp(tree->data, ">=") == 0)
    {
        add_to_str(&g->instructions, "CALL $eval_greater_equal\n");
    } else if (strcmp(tree->data, ">") == 0)
    {
        add_to_str(&g->instructions, "CALL $eval_greater\n");
    } else if (strcmp(tree->data, "<=") == 0)
    {
        add_to_str(&g->instructions, "CALL $eval_less_equal\n");   // TODO
    } else if (strcmp(tree->data, "<") == 0)
    {
        add_to_str(&g->instructions, "CALL $eval_less\n");   // TODO
    } else if (strcmp(tree->data, "==") == 0)
    {
        add_to_str(&g->instructions, "CALL $eval_equals\n");
    } else if (strcmp(tree->data, "!=") == 0)
    {
        add_to_str(&g->instructions, "CALL $eval_not_equals\n");   // TODO
    } else if(strcmp(tree->data, ";") == 0){
        add_to_str(&g->instructions, "INT2FLOATS\n");
    } else if(strcmp(tree->data, "??") == 0){
        add_to_str(&g->instructions, "CALL $double_questmark\n");   // TODO
    } else if(strcmp(tree->data, "!") == 0){
        add_to_str(&g->instructions, "CALL $not_nil\n");   // TODO
    }
    else {
        symtable_item_t *item = SymtableSearchAll(stack, tree->data);
        if (item != NULL) {
            if (stack->size == 1) {
                // We are in global frame
                add_to_str(&g->instructions, "PUSHS GF@");
            } else {
                add_to_str(&g->instructions, "PUSHS LF@");
            }
        }
        else if (tree->type == TYPE_INT) {
            add_to_str(&g->instructions, "PUSHS int@");
        }
        else if (tree->type == TYPE_DOUBLE) {
            add_to_str(&g->instructions, "PUSHS float@");
        }
        else if (tree->type == TYPE_STRING) {
            add_to_str(&g->instructions, "PUSHS string@");
        }
        else if (tree->type == TYPE_BOOL) {
            add_to_str(&g->instructions, "PUSHS bool@");
        }
        else if (tree->type == TYPE_NIL) {
            add_to_str(&g->instructions, "PUSHS nil@");
        }
        add_to_str(&g->instructions, tree->data);
        add_to_str(&g->instructions, "\n");
    }
}

void function_gen(Generator *g, Lexeme *token, symtable_item_t *function){
    add_to_str(&g->instructions, "JUMP $");
    add_to_str(&g->instructions, token->extra_data.string);
    add_to_str(&g->instructions, "_end");
    add_to_str(&g->instructions, "\n");
    add_to_str(&g->instructions, "LABEL $");
    add_to_str(&g->instructions, token->extra_data.string);
    add_to_str(&g->instructions, "\n");
    add_to_str(&g->instructions, "CREATEFRAME\n");
    for (int i = 0; i < function->data->param_count; i++) {
        add_to_str(&g->instructions, "DEFVAR TF@");
        add_to_str(&g->instructions, function->data->params_id[i]);
        add_to_str(&g->instructions, "\n");
    }
    add_to_str(&g->instructions, "PUSHFRAME\n");
    for (int i = 0; i < function->data->param_count; i++) {
        add_to_str(&g->instructions, "POPS LF@");
        add_to_str(&g->instructions, function->data->params_id[i]);
        add_to_str(&g->instructions, "\n");
    }

    // TODO
}

void define_var(Generator *g, Lexeme *token, symtable_stack_t *stack){
    if (stack->size == 1) {
        add_to_str(&g->instructions, "DEFVAR GF@");
    } else {
        add_to_str(&g->instructions, "DEFVAR LF@");
    }
    add_to_str(&g->instructions, token->extra_data.string);
    add_to_str(&g->instructions, "\n");
}

void assign_var_0(Generator *g, Lexeme *token, symtable_stack_t *stack){
    if (stack->size == 1) {
        // We are in global frame
        add_to_str(&g->instructions, "MOVE GF@");
    } else {
        add_to_str(&g->instructions, "MOVE LF@");
    }
    add_to_str(&g->instructions, token->extra_data.string);
    add_to_str(&g->instructions, " ");
    symtable_item_t *item = SymtableSearchAll(stack, token->extra_data.string);
    switch (item->data->item_type)
    {
    case TYPE_INT:
        add_to_str(&g->instructions, "int@");
        break;
    case TYPE_DOUBLE:
        add_to_str(&g->instructions, "float@");
        break;
    case TYPE_STRING:
        add_to_str(&g->instructions, "string@!stack_var");
        break;
    default:
        break;
    }
}

void if_handle(Generator *g, Lexeme *token, symtable_stack_t *stack){
    if (stack->size == 1) {
        // We are in global frame
        add_to_str(&g->instructions, "MOVE GF@");
    } else {
        add_to_str(&g->instructions, "MOVE LF@");
    }
    add_to_str(&g->instructions, token->extra_data.string);
    add_to_str(&g->instructions, " ");
    symtable_item_t *item = SymtableSearchAll(stack, token->extra_data.string);
    switch (item->data->item_type)
    {
    case TYPE_INT:
        add_to_str(&g->instructions, "int@");
        break;
    case TYPE_DOUBLE:
        add_to_str(&g->instructions, "float@");
        break;
    case TYPE_STRING:
        add_to_str(&g->instructions, "string@!stack_var");
        break;
    default:
        break;
    }
}

void assign_var_1(Generator *g, char *key, symtable_stack_t *stack, ast_t *tree, bool is_expression, char *key_func){
    if (is_expression){
        exp_postfix(g, tree, stack);
    } else {
        function_call_gen_prep(g, key_func, g->parameters_count);
        add_to_str(&g->instructions, "CALL $");
        add_to_str(&g->instructions, key_func);
        add_to_str(&g->instructions, "\n");
    }
    if (stack->size == 1){
        add_to_str(&g->instructions, "POPS GF@");    
    } else {
        add_to_str(&g->instructions, "POPS LF@");
    }
    add_to_str(&g->instructions, key);
    add_to_str(&g->instructions, "\n");
}

void return_func_exp(Generator *g, ast_t *tree, symtable_stack_t *stack, char *key_func){
    exp_postfix(g, tree, stack);
    //add_to_str(&g->instructions, "POPS LF@!retval\n");
    add_to_str(&g->instructions, "POPFRAME\n");
    add_to_str(&g->instructions, "RETURN\n");
    add_to_str(&g->instructions, "LABEL $");
    add_to_str(&g->instructions, key_func);
    add_to_str(&g->instructions, "_end\n");
}

void function_call_gen_prep(Generator *g, char *key_func, int params_count){
    for (int i = g->parameters_count-1; i >= 0; i--) {
        add_to_str(&g->instructions, g->parameters[i]);
    }
    
    // CALL WRITE FUNC
    if(strcmp(key_func, "write") == 0){
        add_to_str(&g->instructions, "PUSHS int@");
        char buffer[16];
        sprintf(buffer, "%d", params_count);
        add_to_str(&g->instructions, buffer);     
        // TODO: PUSH NUMBER OF PARAMETERS TO WRITE
        add_to_str(&g->instructions, "\n");
    }

    add_to_str(&g->function_call_tmps, "CALL $");
    add_to_str(&g->function_call_tmps, key_func);     // func name
    add_to_str(&g->function_call_tmps, "\n");

    for (int i = 0; i < g->parameters_count; i++) {
        free(g->parameters[i]);
    }
    g->parameters_count = 0;
    // TODO 
}

void func_call(Generator *g){
    add_to_str(&g->instructions, g->stack_values.str);
    str_clear(&g->stack_values);
    add_to_str(&g->instructions, g->function_call_tmps.str);
    str_clear(&g->function_call_tmps);
}

void func_load_params(Generator *g, Lexeme *token, symtable_item_t *item, symtable_stack_t *stack){
    g->parameters_count++;
    g->parameters = realloc(g->parameters, g->parameters_count * sizeof(char*));
    extract_value(g, token, item, stack);
}
