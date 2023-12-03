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
 * 
 * @param s String of instructions
 * @param str Instruction to be added
 */
void add_to_str(String *s, char *str);

/**
 * @brief Destructor of string
 * 
 * @param s String to be destroyed
 */
void str_dtor(String *s);

void str_clear(String *s);

/**
 * @brief Start of code generator
 * 
 */
void StartGenerator(Generator *g);

/**
 * @brief Cleanup of code generator
 */
void cleanup(Generator *g);

void if_stat(Generator *g);
void else_stat(Generator *g);
void else_if_stat(Generator *g);
void while_stat(Generator *g);


#endif //GENERATOR_H
