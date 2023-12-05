/**
 * @file builtin_func.h
 * @author Marek Joukl (xjoukl00)
 * @brief Header for builtin functions for code generator
 */

#ifndef BUILTIN_FUNC_H
#define BUILTIN_FUNC_H
#include "generator.h"

// Built in function that writes to stdout
void builtin_write(Generator *g);
// Built in function that reads Integer from stdin
void builtin_readInt(Generator *g);
// Built in function that reads Stringfrom stdin
void builtin_readString(Generator *g);
// Built in function that reads Double from stdin
void builtin_readDouble(Generator *g);
// Built in function that converts Integer to String
void built_Int2Double(Generator *g);
// Built in function that converts Double to Integer
void built_Double2Int(Generator *g);
// Built in function that returns length of String
void builtin_length(Generator *g);
void builtin_substr(Generator *g);
// Built in function that returns ASCII value of first character in String(if String is empty, returns 0)
void builtin_ord(Generator *g);
// Built in function that returns ASCII symbol representing given number
void builtin_chr(Generator *g);
// Built in function that evaluates if true or false
void eval_bool(Generator *g);
// check if expressions are equal
void eval_equals(Generator *g);
// check if first expression is greater or equal than second
void eval_greater_equal(Generator *g);
// check if first expression is greater than second
void eval_greater(Generator *g);
// check if first expression is less than second
void eval_less_equal(Generator *g);
// check if first expression is less than second
void eval_less(Generator *g);


#endif // BUILTIN_FUNC_H
