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



// String of instructions
typedef struct{
    char *str;      // string
    int length;     // length of string
    int alloc_size; // allocated size of string
} String;

// Structure of generator
typedef struct{
    String header;          // header of code
    String instructions;    // instructions
    String footer;          // footer of code
    String stack_values;    // values on stack
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
void if_stat(Generator *g);
/**
 * @brief Generate label for else statement
 */
void else_stat(Generator *g);
/**
 * @brief Generate label for while loop
 */
void while_loop_gen(Generator *g);
/**
 * @brief Generate label for while loop end
 */
void while_loop_end(Generator *g);
/**
 * @brief Function that extracts value from token given by parser
 * @param g 
 * @param token 
 */
void extract_value(Generator *g, Lexeme *token);
/**
 * @brief Call builtin write function
 * @param g 
 */
void call_builtin_write(Generator *g);
/**
 * @brief Handle expression in postfix notation given by precedent analysis
 * @param g 
 * @param token 
 */
void exp_postfix(Generator *g, Lexeme *token);
/**
 * @brief Generate label for function
 * @param g 
 * @param token 
 */
void function_gen(Generator *g, Lexeme *token);


#endif //GENERATOR_H
