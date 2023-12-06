/**
 * @file generator.h
 * @author Marek Joukl (xjoukl00)
 * @brief Header for code generator
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#define INIT_STRING_SIZE 8

#include "parser.h"
#include "error.h"
#include "precedent_stack.h"



// String of instructions
typedef struct{
    char *str;      // string
    int length;     // length of string
    int alloc_size; // allocated size of string
} String;

// Structure of generator
typedef struct{
    String header;              // header of code
    String instructions;        // instructions
    String footer;              // footer of code
    String stack_values;        // values on stack
    String function_call_tmps;  // tmps for function call
    String vars;                // global variables
    String function_definitions;// function definitions
    char **parameters;          // parameters of function
    int parameters_count;       // count of parameters
    String temp_string;         // temporary string for one param
} Generator;

/**
 * @brief prints header of code
 */
void print_header(Generator *g);
/**
 * @brief prints footer of code
 */
void print_footer(Generator *g);

/**
 * @brief Initialization of string
 * @param s string to be initialized
 */
void str_init(String *s);

/**
 * @brief Add instruction string to string to be printed
 * @param s String of instructions
 * @param str Instruction to be added
 */
void add_to_str(String *s, char *str);
/**
 * @brief Removes one char from string
 * @param s 
 */
void remove_ch_from_str(String *s);

/**
 * @brief Destructor of string
 * @param s String to be destroyed
 */
void str_dtor(String *s);
/**
 * @brief Clear string
 * @param s 
 */
void str_clear(String *s);

/**
 * @brief Start of code generator
 */
void StartGenerator(Generator *g);

/**
 * @brief Cleanup of code generator
 */
void cleanup_generator(Generator *g);

/**
 * @brief Generate label for if statement
 */
void if_stat(Generator *g, ast_t *asttree, symtable_stack_t *stack, int if_counter);
/**
 * @brief Generate label for else statement
 */
void else_stat(Generator *g, bool is_else_start, int else_counter);
/**
 * @brief Generate label for while loop
 */
void while_loop_gen(Generator *g, symtable_stack_t *stack, ast_t *asttree, int while_counter);
/**
 * @brief Generate label for while loop end
 */
void while_loop_end(Generator *g, int while_counter);
/**
 * @brief Function that extracts value from token given by parser
 * @param g 
 * @param token 
 */
void extract_value(Generator *g, Lexeme *token, symtable_item_t *item, symtable_stack_t *stack);
/**
 * @brief Handle expression in postfix notation given by precedent analysis
 * @param g 
 * @param token 
 */
void assign_var_1(Generator *g, char* key, symtable_stack_t *stack, ast_t *tree, bool is_expression, char *key_func);
/**
 * @brief Generate label for function
 * @param g 
 * @param token 
 */
void function_gen(Generator *g, Lexeme *token, symtable_item_t *function);

void function_call_gen_prep(Generator *g, char *key_func, int param_count);
void func_load_params(Generator *g, Lexeme *token, symtable_item_t *item, symtable_stack_t *stack);
void func_call(Generator *g);
void define_var(Generator *g, Lexeme *token, symtable_stack_t *stack);
void assign_var_0(Generator *g, Lexeme *token, symtable_stack_t *stack);
void exp_postfix(Generator *g, ast_t *tree, symtable_stack_t *stack);
void return_func_exp(Generator *g, ast_t *tree, symtable_stack_t *stack, char *key);


#endif //GENERATOR_H
