#include "scanner.h"
#include "automat.h"
#include <stdio.h>

// if INCLUDE_WHITE_SPACES is 1, then the scanner will return all lexemes, including whitespaces
#define INCLUDE_WHITE_SPACES 0

// returns lexeme.kind as a string, based on the enum value
const char* lexeme_kind_to_string(int kind)
{
    switch (kind)
    {
        case ERROR: return "ERROR";
        case LESS: return "LESS";
        case LESS_EQUAL: return "LESS_EQUAL";
        case GREATER: return "GREATER";
        case GREATER_EQUAL: return "GREATER_EQUAL";
        case EXCLAMATION_POINT: return "EXCLAMATION_POINT";
        case NOT_EQUAL: return "NOT_EQUAL";
        case ASSIGNMENT: return "ASSIGNMENT";
        case EQUAL: return "EQUAL";
        case COMMA: return "COMMA";
        case COLON: return "COLON";
        case RIGHT_PAR: return "RIGHT_PAR";
        case LEFT_PAR: return "LEFT_PAR";
        case DOUBLE_QUESTION_MARK: return "DOUBLE_QUESTION_MARK";
        case IDENTIFIER: return "IDENTIFIER";
        case IDENTIFIER_TYPE_NIL: return "IDENTIFIER_TYPE_NIL";
        case STRING: return "STRING";
        case MULTILINE_STRING: return "MULTILINE_STRING";
        case INTEGER: return "INTEGER";
        case DOUBLE: return "DOUBLE";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case ASTERISK: return "ASTERISK";
        case SLASH: return "SLASH";
        case COMMENT: return "COMMENT";
        case BLOCK_COMMENT: return "BLOCK_COMMENT";
        case SPACE: return "SPACE";
        case NEWLINE: return "NEWLINE";
        case LEX_EOF: return "LEX_EOF";
        default: return "UNKNOWN";
    }
}

int main(void)
{
    Lexeme lexeme;
    while (true)
    {
        if (INCLUDE_WHITE_SPACES == 1) lexeme = get_lexeme();
        else if (INCLUDE_WHITE_SPACES == 0) lexeme = get_next_non_whitespace_lexeme();

        printf("lexeme kind: %s\n", lexeme_kind_to_string(lexeme.kind));
        if (lexeme.kind == LEX_EOF) break;
        if (lexeme.kind == IDENTIFIER || lexeme.kind == STRING || lexeme.kind == MULTILINE_STRING)
        {
            printf("       data: %s\n", lexeme.extra_data.string);
        }
        if (lexeme.kind == INTEGER)
        {
            //printf("%s\n", "dubeg");
            printf("       data: %d\n", lexeme.extra_data.IntValue);
        }
        if (lexeme.kind == DOUBLE)
        {
            printf("       data: %f\n", lexeme.extra_data.DoubleValue);
        }
        printf("       line: %d\n", lexeme.line);
        printf("\n");
    }
    
    return 0;
}


