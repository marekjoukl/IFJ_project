#include "scanner.h"

Lexeme get_lexeme(void)
{
    AutomatState current = Start;
    unsigned int counter = 0;   // helper variable for nested multiline comments
    Lexeme lexeme;
    while(true)
    {
        char edge = getchar();
        if (edge == EOF)
        {
            return (Lexeme) { .kind = LEX_EOF };
        }
        AutomatState next = transition(current, edge, &counter);
        if (next == Error)
        {
            ungetc(edge, stdin);
            lexeme = make_lexeme(current);
            return lexeme;
        }
        current = next;
    }
}

Lexeme make_lexeme(AutomatState current)
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
            break;
        case IdTypeNil:
            lexeme.kind = IDENTIFIER_TYPE_NIL;
            break;
        case EndStringLit:
            lexeme.kind = STRING;
            break;
        case EmptyString:
            lexeme.kind = STRING;
            break;
        case MltLnStringLit:
            lexeme.kind = MULTILINE_STRING;
            break;
        case IntLit:
            lexeme.kind = INTEGER;
            break;
        case DoubleLit:
            lexeme.kind = DOUBLE;
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
        case Error:
            fprintf(stderr, "Error: scanner.c - should have created a token already");
            exit(1);
        case Start:
            fprintf(stderr, "Error: scanner.c - invalid Start transition character");
            exit(1);
        default:
            fprintf(stderr, "Error: scanner.c - not in a final state");
            exit(1);
    }
    return lexeme;
}
