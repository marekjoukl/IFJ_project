/**
 * @file builtin_func.c
 * @author Marek Joukl (xjoukl00)
 * @brief Builtin functions for code generator
 */
#include "builtin_func.h"

void builtin_write(Generator *g){
    add_to_str(&g->header,  "LABEL $write\n"
                            "PUSHFRAME\n"
                            "CREATEFRAME\n"
                            "DEFVAR TF@count\n"             // count of terms to print
                            "POPS TF@count\n"               // TODO: find out how to push number of terms to print
                            "DEFVAR TF@tmp\n"               // TODO: find out how to push term to print
                            "LABEL $write_loop\n"
                            "JUMPIFEQ $write_end TF@count int@0\n"
                            "SUB TF@count TF@count int@1\n"
                            "POPS TF@tmp\n"
                            "WRITE TF@tmp\n"
                            "JUMP $write_loop\n"
                            "LABEL $write_end\n"
                            "PUSHS nil@nil\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void builtin_readInt(Generator *g){
    add_to_str(&g->header,  "LABEL $readInt\n"
                            "PUSHFRAME\n"
                            "CREATEFRAME\n"
                            "DEFVAR TF@tmp\n"
                            "READ TF@tmp int\n"                                         // READ from stdin
                            "DEFVAR TF@?type\n"
                            "TYPE TF@?type TF@tmp\n"                                    // get ?type of read value
                            "JUMPIFNEQ $readIntError_end TF@?type string@int\n"            // if ?type != int, jump to ERROR
                            "PUSHS TF@tmp\n"                                            // TODO: find out how to push read Int
                            "JUMP $readInt_end\n"
                            "LABEL $readIntError_end\n"
                            "PUSHS nil@nil\n"
                            "LABEL $readInt_end\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void builtin_readString(Generator *g){
    add_to_str(&g->header,  "LABEL $readString\n"
                            "PUSHFRAME\n"
                            "CREATEFRAME\n"
                            "DEFVAR TF@tmp\n"
                            "READ TF@tmp string\n"                                      // READ from stdin
                            "DEFVAR TF@?type\n"
                            "TYPE TF@?type TF@tmp\n"                                    // get ?type of read value
                            "JUMPIFNEQ $readStringError_end TF@!type string@string\n"   // if ?type != string, jump to ERROR
                            "PUSHS TF@tmp\n"                                            // TODO: find out how to push read string
                            "JUMP $readString_end\n"
                            "LABEL $readStringError_end\n"
                            "PUSHS nil@nil\n"
                            "LABEL $readString_end\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void builtin_readDouble(Generator *g){
    add_to_str(&g->header,  "LABEL $readDouble\n"
                            "PUSHFRAME\n"
                            "CREATEFRAME\n"
                            "DEFVAR TF@tmp\n"
                            "READ TF@tmp float\n"                                       // READ from stdin
                            "DEFVAR TF@?type\n"
                            "TYPE TF@?type TF@tmp\n"                                    // get ?type of read value
                            "JUMPIFNEQ $readDoubleError_end TF@?type string@float\n"     // if ?type != double, jump to ERROR
                            "PUSHS TF@tmp\n"                                            // TODO: find out how to push read Int
                            "JUMP $readDouble_end\n"
                            "LABEL $readDoubleError_end\n"
                            "PUSHS nil@nil\n"
                            "LABEL $readDuble_end\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void builtin_length(Generator *g){
    add_to_str(&g->header,  "LABEL $length\n"
                            "PUSHFRAME\n"
                            "CREATEFRAME\n"
                            "DEFVAR TF@str\n"
                            "POPS TF@str\n"                 // find out how to store string to stack
                            "DEFVAR TF@?type\n"
                            "TYPE TF@?type TF@str\n"
                            "JUMPIFNEQ !ERROR!CALL!FUNC TF@?type string@string\n"
                            "STRLEN TF@str TF@str\n"        // stores length of string to LF@str
                            "PUSHS TF@str\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void builtin_chr(Generator *g){
    add_to_str(&g->header,  "LABEL $chr\n"
                            "PUSHFRAME\n"
                            "CREATEFRAME\n"
                            "DEFVAR TF@int\n"
                            "POPS TF@int\n"                 // find out how to store int to stack
                            "DEFVAR TF@?type\n"
                            "TYPE TF@?type TF@int\n"
                            "JUMPIFNEQ !ERROR!CALL!FUNC TF@?type string@int\n"     // if not int, ERROR
                            "INT2CHAR TF@int TF@int\n"      // Convert
                            "PUSHS TF@int\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void builtin_ord(Generator *g){
    add_to_str(&g->header,  "LABEL $ord\n"
                            "PUSHFRAME\n"
                            "CREATEFRAME\n"
                            "DEFVAR TF@str\n"
                            "POPS TF@str\n"                         // find out how to store string to stack
                            "DEFVAR TF@?type\n"
                            "TYPE TF@?type TF@str\n"
                            "JUMPIFNEQ $ordError_end TF@?type string@string\n"        // TODO: find out how to return 58, now just returns 0
                            "JUMPIFEQ $ordError_end TF@str string@\n"                 // if empty string, return 0
                            "STRI2INT TF@str TF@str int@0\n"          // Finds value of first char in string
                            "PUSHS TF@str\n"
                            "JUMP $ord_end\n"
                            "LABEL $ordError_end\n"
                            "PUSHS int@0\n"
                            "LABEL $ord_end\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void eval_bool(Generator *g){
    add_to_str(&g->header,  "LABEL $eval_bool\n"
                            "PUSHFRAME\n"
                            "CREATEFRAME\n"
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
                            "PUSHS bool@true\n"
                            "POPFRAME\n"
                            "RETURN\n"
                            "LABEL $bool_fasle\n"
                            "PUSHS bool@false\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void eval_equals(Generator *g){
    add_to_str(&g->header,  "LABEL $eval_equals\n"
                            "PUSHFRAME\n"
                            "CREATEFRAME\n"
                            "POPS GF@!tmp1\n"                               // must be pushed before call
                            "POPS GF@!tmp2\n"                               // must be pushed before call
                            "JUMPIFEQ $equals_true GF@!tmp1 GF@!tmp2\n"     // tmps must be already stored
                            "PUSHS bool@false\n"
                            "POPFRAME\n"
                            "RETURN\n"
                            "LABEL $equals_true\n"
                            "PUSHS bool@true\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void eval_greater_equal(Generator *g){
    add_to_str(&g->header,  "LABEL $eval_greater_equal\n"
                            "PUSHFRAME\n"
                            "CREATEFRAME\n"
                            "POPS GF@!tmp1\n"                                      // must be pushed before call
                            "POPS GF@!tmp2\n"                                      // must be pushed before call
                            "JUMPIFEQ $greater_equal_true GF@!tmp1 GF@!tmp2\n"     // tmps must be already stored
                            "DEFVAR TF@?type1\n"
                            "GT TF@?type1 GF@!tmp1 GF@!tmp2\n"                       // if tmp1 > tmp2, return true
                            "JUMPIFNEQ $greater_equal_true TF@?type1 bool@true\n"
                            "PUSHS bool@false\n"
                            "POPFRAME\n"
                            "RETURN\n"
                            "LABEL $greater_equal_true\n"
                            "PUSHS bool@true\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void eval_greater(Generator *g){
    add_to_str(&g->header,  "LABEL $eval_greater\n"
                            "PUSHFRAME\n"
                            "CREATEFRAME\n"
                            "POPS GF@!tmp1\n"                                      // must be pushed before call
                            "POPS GF@!tmp2\n"                                      // must be pushed before call
                            "DEFVAR TF@?type1\n"
                            "GT TF@?type1 GF@!tmp1 GF@!tmp2\n"                       // if tmp1 > tmp2, return true
                            "JUMPIFNEQ $greater_true TF@?type1 bool@true\n"
                            "PUSHS bool@false\n"
                            "POPFRAME\n"
                            "RETURN\n"
                            "LABEL $greater_true\n"
                            "PUSHS bool@true\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void eval_less_equal(Generator *g){
    add_to_str(&g->header,  "LABEL $eval_less_equal\n"
                            "PUSHFRAME\n"
                            "CREATEFRAME\n"
                            "POPS GF@!tmp1\n"                                      // must be pushed before call
                            "POPS GF@!tmp2\n"                                      // must be pushed before call
                            "JUMPIFEQ $greater_less_true GF@!tmp1 GF@!tmp2\n"     // tmps must be already stored
                            "DEFVAR TF@?type1\n"
                            "LT TF@?type1 GF@!tmp1 GF@!tmp2\n"                       // if tmp1 > tmp2, return true
                            "JUMPIFNEQ $greater_less_true TF@?type1 bool@true\n"
                            "PUSHS bool@false\n"
                            "POPFRAME\n"
                            "RETURN\n"
                            "LABEL $greater_less_true\n"
                            "PUSHS bool@true\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void eval_less(Generator *g){
    add_to_str(&g->header,  "LABEL $eval_less\n"
                            "PUSHFRAME\n"
                            "CREATEFRAME\n"
                            "POPS GF@!tmp1\n"                                      // must be pushed before call
                            "POPS GF@!tmp2\n"                                      // must be pushed before call
                            "DEFVAR TF@?type1\n"
                            "LT TF@?type1 GF@!tmp1 GF@!tmp2\n"                       // if tmp1 > tmp2, return true
                            "JUMPIFNEQ $less_true TF@?type1 bool@true\n"
                            "PUSHS bool@false\n"
                            "POPFRAME\n"
                            "RETURN\n"
                            "LABEL $less_true\n"
                            "PUSHS bool@true\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

void eval_not_equals(Generator *g){
    add_to_str(&g->header,  "LABEL $eval_not_equals\n"
                            "PUSHFRAME\n"
                            "CREATEFRAME\n"
                            "POPS GF@!tmp1\n"                               // must be pushed before call
                            "POPS GF@!tmp2\n"                               // must be pushed before call
                            "JUMPIFNEQ $not_equals_true GF@!tmp1 GF@!tmp2\n"     // tmps must be already stored
                            "PUSHS bool@false\n"
                            "POPFRAME\n"
                            "RETURN\n"
                            "LABEL $not_equals_true\n"
                            "PUSHS bool@true\n"
                            "POPFRAME\n"
                            "RETURN\n");
}

