#ifndef ERROR_H
#define ERROR_H

#define LEXICAL_ERROR 1             // lexical error
#define SYNTAX_ERROR 2              // syntax error   
#define DEFINITION_ERROR 3          // undefined function or redefinition of variable
#define PARAMETER_TYPE_ERROR 4      // wrong number or type of parameters
#define UNDEFINED_VAR_ERROR 5       // use of undefined or uninitialized variable
#define RETURN_ERROR 6              // return statement in void function or no return statement in non-void function
#define TYPE_ERROR 7                // wrong type of operands in arithmetic, string or relational expression
#define TYPE_DEDUCTION_ERROR 8      // type of variable or parameter is not specified and cannot be deduced
#define OTHER_SEMANTIC_ERROR 9      // other semantic errors
#define INTERNAL_ERROR 99           // internal error - memory allocation, etc.

#endif // ERROR_H
