#include "scanner.h"

char* keywords[] = { "Double", "else", "func", "if", "Int", "let", "nil", "return", "String", "var", "while"};

Lexeme get_lexeme(void)
{
    Lexeme lexeme;              // the lexeme we are going to return
    AutomatState current = Start;
    unsigned int counter = 0;   // helper variable for nested multiline comments
    
    bool storing_data = false;  // helper variable for storing extra data
    char *str = NULL;           // helper variable for storing extra data
    size_t len = 0;             // helper variable for storing extra data

    while(true)
    {
        char edge = getchar();
        if (edge == EOF)
        {
            return (Lexeme) { .kind = LEX_EOF };
        }

        AutomatState next = transition(current, edge, &counter);

        // if we are in the first state and we need to extra data
        if (current == Start && (next == IntLit || next == BeginString || next == Id)) storing_data = true; 
        
        if (storing_data == true)
        {
            add_to_string(str, &len, edge, current, next);
            if (str == NULL)
            {
                fprintf(stderr, "Error: scanner.c - realloc failed");
                exit (99);      // EXIT CODE 99 - failed to allocate memory
            }
        }

        if (next == Error)
        {
            ungetc(edge, stdin);
            lexeme = make_lexeme(current, str);
            if (lexeme.kind == ERROR) 
            {
                if (str != NULL) free(str);
                fprintf(stderr, "Error: scanner.c - wrong lexeme structure");
                exit (1);       // EXIT CODE 1 - wrong lexeme structure
            }
            return lexeme;
        }
        current = next;
    }
}

// helper function for storing extra data, 
void add_to_string(char *str, size_t *len, char edge, AutomatState current, AutomatState next)
{
    str = realloc(str, sizeof(char) * (*len + 1));
    if (str == NULL)
    {
        return;
    }

    // add string terminator, if exiting a final state
    if (next == Error && 
       (current == EndStringLit || current == EndMltLnStringLit || current == EmptyString || 
        current == IntLit || current == DoubleLit ||current == Id || current == IdTypeNil))
    {
        str[*len] = '\0';
        (*len)++;
        return;
    }
    str[*len] = edge;
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
        case DoubleQmark:
            lexeme.kind = DOUBLE_QUESTION_MARK;
            break;
        case Id:
            lexeme.kind = IDENTIFIER;
            lexeme.extra_data.string = str;
            break;
        case IdTypeNil:
            lexeme.kind = IDENTIFIER_TYPE_NIL;
            lexeme.extra_data.string = str;
            break;
        case EndStringLit:
            lexeme.kind = STRING;
            lexeme.extra_data.string = str;
            break;
        case EmptyString:
            lexeme.kind = STRING;
            lexeme.extra_data.string = str;
            break;
        case MltLnStringLit:
            lexeme.kind = MULTILINE_STRING;
            lexeme.extra_data.string = str;
            break;
        case IntLit:
            lexeme.kind = INTEGER;
            lexeme.extra_data.IntValue = atoi(str);
            break;
        case DoubleLit:
            lexeme.kind = DOUBLE;
            lexeme.extra_data.DoubleValue = atof(str);
            break;
        case Plus:
            lexeme.kind = PLUS;
            break;
        case Minus:
            lexeme.kind = MINUS;
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
