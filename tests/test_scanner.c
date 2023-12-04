#include "../src/scanner.h"
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
        case LEFT_BRACKET: return "LEFT_BRACKET";
        case RIGHT_BRACKET: return "RIGHT_BRACKET";
        case DOUBLE_QUESTION_MARK: return "DOUBLE_QUESTION_MARK";
        case IDENTIFIER: return "IDENTIFIER";
        case STRING_LIT: return "STRING_LIT";
        case INTEGER_LIT: return "INTEGER_LIT";
        case DOUBLE_LIT: return "DOUBLE_LIT";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case ARROW: return "ARROW";
        case ASTERISK: return "ASTERISK";
        case SLASH: return "SLASH";
        case COMMENT: return "COMMENT";
        case BLOCK_COMMENT: return "BLOCK_COMMENT";
        case SPACE: return "SPACE";
        case NEWLINE: return "NEWLINE";
        case LEX_EOF: return "LEX_EOF";
        case DOUBLE: return "DOUBLE";
        case ELSE: return "ELSE";
        case FUNC: return "FUNC";
        case IF: return "IF";
        case INT: return "INT";
        case LET: return "LET";
        case NIL: return "NIL";
        case RETURN: return "RETURN";
        case STRING: return "STRING";
        case VAR: return "VAR";
        case WHILE: return "WHILE";
        case UNDERSCORE: return "UNDERSCORE";
        default: return "UNKNOWN";
    }
}

int main(int argc, char *argv[])
{
    FILE *input = stdin;
    if (argc > 1) {
        fprintf(stderr, "Opening file %s\n", argv[1]);
        input = fopen(argv[1], "r");
        if (input == NULL) {
            printf("Could not open file %s\n", argv[1]);
            return 1;
        }
    }

    fprintf(stderr, "START of string or identifier ->|ID/STRING|<- END of string or identifier\n\n");
    Lexeme lexeme;
    for (int i = 0; i < 2; i++)
    {
    while (true)
    {
        if (INCLUDE_WHITE_SPACES == 1) lexeme = get_lexeme();
        else if (INCLUDE_WHITE_SPACES == 0) lexeme = get_next_non_whitespace_lexeme();

        fprintf(stderr, "lexeme kind: %s\n", lexeme_kind_to_string(lexeme.kind));
        if (lexeme.kind == LEX_EOF) break;
        if (lexeme.kind == IDENTIFIER || lexeme.kind == STRING_LIT || lexeme.kind == INTEGER_LIT || lexeme.kind == DOUBLE_LIT)
        {
            fprintf(stderr, "       data: ->|%s|<-\n", lexeme.extra_data.string);
            //free(lexeme.extra_data.string); // free the string, because it was allocated by the scanner
        }
        if (lexeme.kind == DOUBLE) fprintf(stderr, "       nil_type: %s\n", lexeme.nil_type ? "true" : "false");
        if (lexeme.kind == INT) fprintf(stderr, "       nil_type: %s\n", lexeme.nil_type ? "true" : "false");
        if (lexeme.kind == STRING) fprintf(stderr, "       nil_type: %s\n", lexeme.nil_type ? "true" : "false");
        
        fprintf(stderr, "       line: %d\n", lexeme.line);
        fprintf(stderr, "\n");
    }
    }
    fprintf(stderr, "\nSTART of string or identifier ->|ID/STRING|<- END of string or identifier\n");

    if (argc > 1) {
        fclose(input);
    }
    
    return 0;
}


