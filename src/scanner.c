#include "scanner.h"

#define STRING_CHUNK 10   // how much new space to allocate when reallocating space for a string

// return the next lexeme, skipping whitespaces
Lexeme get_next_non_whitespace_lexeme(void)
{
    Lexeme lexeme;
    do
    {
        lexeme = get_lexeme();
    } while (lexeme.kind == SPACE || lexeme.kind == NEWLINE ||lexeme.kind == COMMENT || lexeme.kind == BLOCK_COMMENT);
    return lexeme;
}

// return the next lexeme, including whitespaces
Lexeme get_lexeme(void)
{
    static unsigned int line_counter = 1;   // helper variable for counting lines
    Lexeme lexeme;              // the lexeme we are going to return
    AutomatState current = Start;
    
    bool storing_data = false;  // helper variable for storing extra data
    char *str = NULL;           // helper variable for storing strings
    size_t len = 0;             // helper variable for storing strings
    size_t capacity = 0;        // helper variable for storing strings

    while(true)
    {
        char edge = getchar();

        AutomatState next = transition(current, edge);

        // if we are in the first state and we need to extra data
        if (current == Start && (next == IntLit || next == BeginString || next == Id)) storing_data = true; 
        
        if (storing_data == true)
        {
            add_to_string(&str, &len, &capacity, edge, current, next);
            if (str == NULL)
            {
                fprintf(stderr, "Error: scanner.c - realloc failed\n");
                exit (99);      // EXIT CODE 99 - failed to allocate memory
            }
        }

        if (next == Error)
        {
            ungetc(edge, stdin);
            lexeme = make_lexeme(current, str);
            lexeme.line = line_counter;
            if (lexeme.kind == ERROR) 
            {
                if (str != NULL) free(str);
                fprintf(stderr, "Error: scanner.c - wrong lexeme structure at line %i\n", line_counter);
                exit (1);       // EXIT CODE 1 - wrong lexeme structure
            }

            return lexeme;
        }
        current = next;
        if (edge == '\n') (line_counter)++;
    }
}

// helper function for storing extra data, 
void add_to_string(char **str, size_t *len, size_t *capacity, char edge, AutomatState current, AutomatState next)
{
    // if we need to allocate more space
    if (*len >= *capacity)
    {
        *str = realloc(*str, sizeof(char) * (*len + STRING_CHUNK));
        if (*str == NULL)
        {
            return;
        }
        (*capacity) += STRING_CHUNK;
    }

    // add string terminator, if exiting a final state
    if (next == Error && 
       (current == EndStringLit || current == EndMltLnStringLit || current == EmptyString || 
        current == IntLit || current == DoubleLitDec || current == DoubleLitExp || current == Id))
    {
        (*str)[*len] = '\0';
        (*len)++;
        return;
    }
    (*str)[*len] = edge;
    (*len)++;
}

// generates lexeme from the current state
Lexeme make_lexeme(AutomatState current, char *str)
{
    Lexeme lexeme;
    switch (current)
    {
        case Ls:
            lexeme.kind = LESS;
            break;
        case LsEqual:
            lexeme.kind = LESS_EQUAL;
            break;
        case Gt:
            lexeme.kind = GREATER;
            break;
        case GtEqual:
            lexeme.kind = GREATER_EQUAL;
            break;
        case ExlPoint:
            lexeme.kind = EXCLAMATION_POINT;
            break;
        case NotEqual:
            lexeme.kind = NOT_EQUAL;
            break;
        case Assign:
            lexeme.kind = ASSIGNMENT;
            break;
        case Equal:
            lexeme.kind = EQUAL;
            break;
        case Comma:
            lexeme.kind = COMMA;
            break;
        case Colon:
            lexeme.kind = COLON;
            break;
        case RPar:
            lexeme.kind = RIGHT_PAR;
            break;
        case LPar:
            lexeme.kind = LEFT_PAR;
            break;
        case LBrac:
            lexeme.kind = LEFT_BRACKET;
            break;
        case RBrac:
            lexeme.kind = RIGHT_BRACKET;
            break;
        case SingleQmark:
            lexeme.kind = QUESTION_MARK;
            break;
        case DoubleQmark:
            lexeme.kind = DOUBLE_QUESTION_MARK;
            break;
        case Id:
            if (strcmp(str, "Double") == 0) lexeme.kind = DOUBLE;
            else if (strcmp(str, "else") == 0) lexeme.kind = ELSE;
            else if (strcmp(str, "func") == 0) lexeme.kind = FUNC;
            else if (strcmp(str, "if") == 0) lexeme.kind = IF;
            else if (strcmp(str, "Int") == 0) lexeme.kind = INT;
            else if (strcmp(str, "let") == 0) lexeme.kind = LET;
            else if (strcmp(str, "nil") == 0) lexeme.kind = NIL;
            else if (strcmp(str, "return") == 0) lexeme.kind = RETURN;
            else if (strcmp(str, "String") == 0) lexeme.kind = STRING;
            else if (strcmp(str, "var") == 0) lexeme.kind = VAR;
            else if (strcmp(str, "while") == 0) lexeme.kind = WHILE;
            else if (strcmp(str, "_") == 0) lexeme.kind = UNDERSCORE;
            else 
            {
                lexeme.kind = IDENTIFIER;
                lexeme.extra_data.string = str;
                break;
            }
            free(str);
            break;
        case EndStringLit:
            lexeme.kind = STRING_LIT;
            lexeme.extra_data.string = str;
            break;
        case EmptyString:
            lexeme.kind = STRING_LIT;
            lexeme.extra_data.string = str;
            break;
        case EndMltLnStringLit:
            lexeme.kind = MULTILINE_STRING_LIT;
            lexeme.extra_data.string = str;
            break;
        case IntLit:
            lexeme.kind = INTEGER_LIT;
            lexeme.extra_data.IntValue = atoi(str);
            free(str);
            break;
        case DoubleLitDec:
            lexeme.kind = DOUBLE_LIT;
            lexeme.extra_data.DoubleValue = atof(str);
            free(str);
            break;
        case DoubleLitExp:
            lexeme.kind = DOUBLE_LIT;
            lexeme.extra_data.DoubleValue = atof(str);
            free(str);
            break;
        case Plus:
            lexeme.kind = PLUS;
            break;
        case Minus:
            lexeme.kind = MINUS;
            break;
        case Arrow:
            lexeme.kind = ARROW;
            break;
        case Asterisk:
            lexeme.kind = ASTERISK;
            break;
        case Slash:
            lexeme.kind = SLASH;
            break;
        case Space:
            lexeme.kind = SPACE;
            break;
        case NewLine:
            lexeme.kind = NEWLINE;
            break;
        case EndOfFile:
            lexeme.kind = LEX_EOF;
            break;
        case Comment:
            lexeme.kind = COMMENT;
            break;
        case BlockComment:
            lexeme.kind = BLOCK_COMMENT;
            break;
        default:
            lexeme.kind = ERROR;
            break;
    }
    return lexeme;
}
