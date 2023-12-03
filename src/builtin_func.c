/**
 * @file builtin_func.c
 * @author Marek Joukl (xjoukl00)
 * @brief Builtin functions for code generator
 */
#include "builtin_func.h"

void builtin_write(Generator *g){
    add_to_str(&g->header,  "LABEL $write\n"
                            "CREATEFRAME\n"
                            "PUSHFRAME\n"
                            "DEFVAR LF@count\n"             // count of terms to print
                            "DEFVAR LF@tmp\n"               // term to be printed
                            "POPS LF@count\n"               // TODO: find out how to push number of terms to print
                            "LABEL $write_loop\n"
                            "JUMPIFEQ $write_end LF@count int@0\n"
                            "SUB LF@count LF@count int@1\n"
                            "POPS LF@tmp\n"                 // TODO: find out how to push term to print
                            "WRITE LF@tmp\n"
                            "JUMP $write_loop\n"
                            "LABEL $write_end\n"
                            "PUSHS nil@nil\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void builtin_readInt(Generator *g){
    add_to_str(&g->header,  "LABEL $readInt\n"
                            "CREATEFRAME\n"
                            "PUSHFRAME\n"
                            "DEFVAR LF@tmp\n"
                            "READ LF@tmp int\n"                                         // READ from stdin
                            "DEFVAR LF@?type\n"
                            "TYPE LF@?type LF@tmp\n"                                    // get ?type of read value
                            "JUMPIFNEQ $readIntError_end LF@?type string@int\n"            // if ?type != int, jump to ERROR                         
                            "PUSHS LF@tmp\n"                                            // TODO: find out how to push read Int
                            "JUMP $readInt_end\n"
                            "LABEL $readIntError_end\n"
                            "PUSHS nil@nil\n"
                            "LABEL $readInt_end\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void builtin_readString(Generator *g){
    add_to_str(&g->header,  "LABEL $readString\n"
                            "CREATEFRAME\n"
                            "PUSHFRAME\n"
                            "DEFVAR LF@tmp\n"
                            "READ LF@tmp string\n"                                      // READ from stdin
                            "DEFVAR LF@?type\n"
                            "TYPE LF@?type LF@tmp\n"                                    // get ?type of read value
                            "JUMPIFNEQ $readStringError_end LF@!type string@string\n"   // if ?type != string, jump to ERROR                         
                            "PUSHS LF@tmp\n"                                            // TODO: find out how to push read string
                            "JUMP $readString_end\n"
                            "LABEL $readStringError_end\n"
                            "PUSHS nil@nil\n"
                            "LABEL $readString_end\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void builtin_readDouble(Generator *g){
    add_to_str(&g->header,  "LABEL $readDouble\n"
                            "CREATEFRAME\n"
                            "PUSHFRAME\n"
                            "DEFVAR LF@tmp\n"
                            "READ LF@tmp float\n"                                       // READ from stdin
                            "DEFVAR LF@?type\n"
                            "TYPE LF@?type LF@tmp\n"                                    // get ?type of read value
                            "JUMPIFNEQ $readDoubleError_end LF@?type string@float\n"     // if ?type != double, jump to ERROR                         
                            "PUSHS LF@tmp\n"                                            // TODO: find out how to push read Int
                            "JUMP $readDouble_end\n"
                            "LABEL $readDoubleError_end\n"
                            "PUSHS nil@nil\n"
                            "LABEL $readDuble_end\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void builtin_length(Generator *g){
    add_to_str(&g->header,  "LABEL $length\n"
                            "CREATEFRAME\n"
                            "PUSHFRAME\n"
                            "DEFVAR LF@str\n"
                            "POPS LF@str\n"                 // find out how to store string to stack
                            "DEFVAR LF@?type\n"
                            "TYPE LF@?type LF@str\n"
                            "JUMPIFNEQ !ERROR!CALL!FUNC LF@?type string@string\n"
                            "STRLEN LF@str LF@str\n"        // stores length of string to LF@str
                            "PUSHS LF@str\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void builtin_chr(Generator *g){
    add_to_str(&g->header,  "LABEL $chr\n"
                            "CREATEFRAME\n"
                            "PUSHFRAME\n"
                            "DEFVAR LF@int\n"
                            "POPS LF@int\n"                 // find out how to store int to stack
                            "DEFVAR LF@?type\n"
                            "TYPE LF@?type LF@int\n"
                            "JUMPIFNEQ !ERROR!CALL!FUNC LF@?type string@int\n"     // if not int, ERROR
                            "INT2CHAR LF@int LF@int\n"      // Convert
                            "PUSHS LF@int\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void builtin_ord(Generator *g){
    add_to_str(&g->header,  "LABEL $ord\n"
                            "CREATEFRAME\n"
                            "PUSHFRAME\n"
                            "DEFVAR LF@str\n"
                            "POPS LF@str\n"                         // find out how to store string to stack
                            "DEFVAR LF@?type\n"
                            "TYPE LF@?type LF@str\n"
                            "JUMPIFNEQ $ordError_end LF@?type string@string\n"        // TODO: find out how to return 58, now just returns 0
                            "JUMPIFEQ $ordError_end LF@str string@\n"                 // if empty string, return 0
                            "STRI2INT LF@str LF@str int@int\n"          // Finds value of first char in string
                            "PUSHS LF@str\n"
                            "JUMP $ord_end\n"
                            "LABEL $ordError_end\n"
                            "PUSHS int@0\n"
                            "LABEL $ord_end\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void eval_bool(Generator *g){
    add_to_str(&g->header,  "LABEL $eval_bool\n"
                            "CREATEFRAME\n"
                            "PUSHFRAME\n"
                            "TYPE GF@!TYPE1 GF@!tmp1\n"             // must be pushed before call
                            "JUMPIFEQ $bool_string GF@!TYPE1 string@string\n"
                            "JUMPIFEQ $bool_int GF@!TYPE1 string@int\n"
                            "JUMPIFEQ $bool_double GF@!TYPE1 string@float\n"
                            "JUMPIFEQ $bool_false GF@!TYPE1 string@nil\n"
                            "LABEL $bool_string\n"
                            "JUMPIFEQ $bool_false GF@!tmp1 string@\n"       // if empty string
                            "JUMPIFEQ $bool_false GF@!tmp1 string@0\n"      // if string == '0'
                            "JUMP $bool_true\n"     // true
                            "LABEL $bool_int\n"
                            "JUMPIFEQ $bool_false GF@!tmp1 int@0\n"         // if int == 0
                            "JUMP $bool_true\n"     // true
                            "LABEL $bool_double\n"
                            "JUMPIFEQ $bool_false GF@!tmp1 float@0x0p+0\n"  // if double == 0.0
                            "LABEL $bool_true\n"
                            "MOVE GF@!tmp1 bool@true\n"
                            "POPFRAME\n"
                            "RETURN\n"
                            "LABEL $bool_fasle\n"
                            "MOVE GF@!tmp1 bool@false\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void eval_equals(Generator *g){
    add_to_str(&g->header,  "LABEL $eval_equals\n"
                            "CREATEFRAME\n"
                            "PUSHFRAME\n"           // TODO: maybe check for types (if not already checked in parser)
                            "JUMPIFEQ $equals_true GF@!tmp1 GF@!tmp2\n"     // tmps must be already stored
                            "MOVE GF@!tmp1 bool@false\n"
                            "POPFRAME\n"
                            "RETURN\n"
                            "LABEL $equals_true\n"
                            "MOVE GF@!tmp1 bool@true\n"
                            "POPFRAME\n"
                            "RETURN\n");
}



