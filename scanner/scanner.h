#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "automat.h"

char* keywords[] = { "Double", "else", "func", "if", "Int", "let", "nil", "return", "String", "var", "while"};

typedef struct Lexeme
{
    enum kind
    {
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
} Lexeme;

Lexeme make_lexeme(AutomatState current);
Lexeme get_lexeme(void);
