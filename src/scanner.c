#include "scanner.h"

#define STRING_CHUNK 10   // how much new space to allocate when reallocating space for a string
#define BUFFER_CHUNK 10   // how much new space to allocate when reallocating space for the buffer

Buffer buffer = { .tokens = NULL, .capacity = 0, .size = 0, .index = 0 }; // global buffer for storing lexemes

// returns the next lexeme or new line, skipping whitespaces and comments
Lexeme get_next_new_line_or_lexeme(void)
{
    Lexeme lexeme;
    do 
    {
        lexeme = get_lexeme();
    } while (lexeme.kind == SPACE || lexeme.kind == COMMENT || lexeme.kind == BLOCK_COMMENT);
    return lexeme;
}

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
    if (buffer.index < buffer.size)
    {
        return buffer.tokens[buffer.index++];
    }

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
                exit (INTERNAL_ERROR);      // EXIT CODE 99 - failed to allocate memory
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
                exit (LEXICAL_ERROR);       // EXIT CODE 1 - wrong lexeme structure
            }

            // allocate more space for the buffer if needed
            if (buffer.size >= buffer.capacity)
            {
                buffer.tokens = realloc(buffer.tokens, sizeof(Lexeme) * (buffer.size + BUFFER_CHUNK));
                if (buffer.tokens == NULL)
                {
                    fprintf(stderr, "Error: scanner.c - realloc failed\n");
                    exit (INTERNAL_ERROR);      // EXIT CODE 99 - failed to allocate memory
                }
                buffer.capacity += BUFFER_CHUNK;

            }
            buffer.tokens[buffer.size++] = lexeme;
            buffer.index = buffer.size;

            if (lexeme.kind == LEX_EOF) 
            {
                buffer.index = 0;
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
       (current == EmptyString || 
        current == IntLit || current == DoubleLitDec || current == DoubleLitExp || current == Id || current == IdNil))
    {
        (*str)[*len] = '\0';
        (*len)++;
        return;
    }
    else if (next == Error)
    {
        if (current == EndStringLit) // remove " from the end of a string
        {
            (*str)[*len - 1] = '\0';
            (*len)++;
            return;
        }
        if (current == EndMltLnStringLit) // remove \n""" from the end of a multiline string 
        {
            (*str)[*len - 4] = '\0';
            (*len)++;
            return;
        }
    }
    // do not add " on the beginning of a string
    if (!(next == BeginString && current == Start) && !(next == EmptyString && current == BeginString) && 
        !(next == MltLnStringStart && current == EmptyString) && !(next == MltLnStringLit && current == MltLnStringStart))
    {
        (*str)[*len] = edge;
        (*len)++;
    }
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
        case DoubleQmark:
            lexeme.kind = DOUBLE_QUESTION_MARK;
            break;
        case Id:
            if (strcmp(str, "Double") == 0) 
            {
                lexeme.kind = DOUBLE;
                lexeme.nil_type = false;
            }
            else if (strcmp(str, "Int") == 0) 
            {
                lexeme.kind = INT;
                lexeme.nil_type = false;
            }
            else if (strcmp(str, "String") == 0) 
            {
                lexeme.kind = STRING;
                lexeme.nil_type = false;
            }
            else if (strcmp(str, "else") == 0) lexeme.kind = ELSE;
            else if (strcmp(str, "func") == 0) lexeme.kind = FUNC;
            else if (strcmp(str, "if") == 0) lexeme.kind = IF;
            else if (strcmp(str, "let") == 0) lexeme.kind = LET;
            else if (strcmp(str, "nil") == 0) lexeme.kind = NIL;
            else if (strcmp(str, "return") == 0) lexeme.kind = RETURN;
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
        case IdNil:
            if (strcmp(str, "Double?") == 0)
            {
                lexeme.kind = DOUBLE;
                lexeme.nil_type = true;
            }
            else if (strcmp(str, "Int?") == 0)
            {
                lexeme.kind = INT;
                lexeme.nil_type = true;
            }
            else if (strcmp(str, "String?") == 0)
            {
                lexeme.kind = STRING;
                lexeme.nil_type = true;
            }
            else
            {
                lexeme.kind = ERROR;
                break;
            }
            free(str);
            break;
        case EndStringLit:
            lexeme.kind = STRING_LIT;
            lexeme.extra_data.string = str;
            lexeme.is_multiline_comment = false;
            break;
        case EmptyString:
            lexeme.kind = STRING_LIT;
            lexeme.extra_data.string = str;
            lexeme.is_multiline_comment = false;
            break;
        case EndMltLnStringLit:
            lexeme.kind = STRING_LIT;
            lexeme.extra_data.string = str;
            lexeme.is_multiline_comment = true;
            break;
        case IntLit:
            lexeme.kind = INTEGER_LIT;
            lexeme.extra_data.string = str;
            break;
        case DoubleLitDec:
            lexeme.kind = DOUBLE_LIT;
            lexeme.extra_data.string = str;
            break;
        case DoubleLitExp:
            lexeme.kind = DOUBLE_LIT;
            lexeme.extra_data.string = str;
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
        case InvalidInt:
            lexeme.kind = ERROR;
            break;
        default:
            lexeme.kind = ERROR;
            break;
    }
    return lexeme;
}
