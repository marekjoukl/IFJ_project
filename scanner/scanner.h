#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "automat.h"

extern char* keywords[];

typedef struct Lexeme
{
    enum kind
    {
        ERROR,
        LESS,
        LESS_EQUAL,
        GREATER,
        GREATER_EQUAL,
        EXCLAMATION_POINT,
        NOT_EQUAL,
        ASSIGNMENT,
        EQUAL,
        COMMA,
        COLON,
        RIGHT_PAR,
        LEFT_PAR,
        DOUBLE_QUESTION_MARK,
        IDENTIFIER,
        IDENTIFIER_TYPE_NIL,
        STRING,
        MULTILINE_STRING,
        INTEGER,
        DOUBLE,
        PLUS,
        MINUS,
        ASTERISK,
        SLASH,
        COMMENT,
        BLOCK_COMMENT,
        SPACE,
        NEWLINE,
        LEX_EOF,
    } kind;

    union extra_data
    {
        char* string;
        int IntValue;
        double DoubleValue;
    }   extra_data;

    unsigned int line;
} Lexeme;

Lexeme make_lexeme(AutomatState current, char *str);
Lexeme get_lexeme(void);
Lexeme get_next_non_whitespace_lexeme(void);
void add_to_string(char **str, size_t *len, char edge, AutomatState current, AutomatState next);

#endif // SCANNER_H
