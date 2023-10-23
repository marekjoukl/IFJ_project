#include "scanner.h"

AutomatState transition(AutomatState current, char edge, unsigned int *counter)
{
    switch (current)
    {
        //##############################
        // First level states
        // Error, Start, Ls, Gt, ExlPoint, Assign, Comma, Colon, RPar, LPar, SingleQmark, Id, BeginString, IntLit, Plus, Minus, Asterisk, Slash, Space, NewLine
        //##############################
        case Error:
            return Error;

        case Start:
            if (edge == '<') return Ls;
            if (edge == '>') return Gt;
            if (edge == '!') return ExlPoint;
            if (edge == '=') return Assign;
            if (edge == ',') return Comma;
            if (edge == ':') return Colon;
            if (edge == ')') return RPar;
            if (edge == '(') return LPar;
            if (edge == '?') return SingleQmark;
            if (isalpha(edge) || edge == '_') return Id;
            if (edge == '"') return BeginString;
            if (isdigit(edge)) return IntLit;
            if (edge == '+') return Plus;
            if (edge == '-') return Minus;
            if (edge == '*') return Asterisk;
            if (edge == '/') return Slash;
            if (edge == ' ' || edge == '\t' || edge == '\f') return Space;
            if (edge == '\n' || edge == '\r') return NewLine;
            return Error;

        case Ls:
            if (edge == '=') return LsEqual;
            return Error;

        case Gt:
            if (edge == '=') return GtEqual;
            return Error;

        case ExlPoint:
            if (edge == '=') return NotEqual;
            return Error;

        case Assign:
            if (edge == '=') return Equal;
            return Error;

        case Comma:
            return Error;

        case Colon:
            return Error;

        case RPar:
            return Error;

        case LPar:
            return Error;

        case SingleQmark:
            if (edge == '?') return DoubleQmark;
            return Error;

        //if Id is the end state check if it is a keyword
        case Id:
            if (isalnum(edge) || edge == '_') return Id;
            if (edge == '?') return IdTypeNil;
            return Error;

        case BeginString:
            if (edge == '"') return EmptyString;
            if (edge == '\\') return EscSeq;
            return StringLit;

        case IntLit:
            if (isdigit(edge)) return IntLit;
            if (edge == '.') return DecPoint;
            if (edge == 'e' || edge == 'E') return Exp;
            return Error;

        case Plus:
            return Error;

        case Minus:
            return Error;

        case Asterisk:
            return Error;

        case Slash:
            if (edge == '/') return Comment;
            if (edge == '*') {
                (*counter) = 0;
                return CommentBody;
            }
            return Error;

        case Space:
            if (edge == ' ' || edge == '\t' || edge == '\f') return Space;
            return Error;

        case NewLine:
            if (edge == '\n' || edge == '\r') return NewLine;
            return Error;

        //##############################
        // Second level states
        // LsEqual, GtEqual, NotEqual, Equal, DoubleQmark, IdTypeNil, EmptyString, StringLit, DecPoint, Exp, Comment, CommentBody, EscSeq
        //##############################
        case LsEqual:
            return Error;

        case GtEqual:
            return Error;

        case NotEqual:
            return Error;

        case Equal:
            return Error;

        case DoubleQmark:
            return Error;
        
        //check if it is Double? / Int? / String?, else it is an error
        case IdTypeNil:
            return Error;

        case EmptyString:
            if (edge == '"') return MltLnStringLit;
            return Error;

        case StringLit:
            if (edge == '"') return EndStringLit;
            if (edge == '\\') return EscSeq;
            return StringLit;

        case DecPoint:
            if (isdigit(edge)) return DoubleLit;
            return Error;

        case Exp:
            if (isdigit(edge)) return DoubleLit;
            if (edge == '+' || edge == '-') return ExpSign;
            return Error;

        case Comment:
            if (edge != '\n' || edge != '\r' || edge != EOF) return Comment;
            return Error;

        case CommentBody:
            if (edge == '/') return NestedComment;
            if (edge == '*') return CommentEnding;
            return CommentBody;

        case EscSeq:
            if (edge == 'u') return EscU;
            if (edge == 'n' || edge == 'r' || edge == 't' || edge == '"' || edge == '\\') return StringLit;
            return Error;

        //##############################
        // Third level states
        // DoubleLit, ExpSign, NestedComment, CommentEnding, EscU, MltLnStringLit, EndStringLit
        //##############################
        case DoubleLit:
            if (isdigit(edge)) return DoubleLit;
            if (edge == 'e' || edge == 'E') return Exp;
            return Error;

        case ExpSign:
            if (isdigit(edge)) return DoubleLit;
            return Error;

        case NestedComment:
            if (edge == '*') {
                (*counter)++;
                return CommentBody;
            }
            return CommentBody;

        case CommentEnding:
            if (edge == '/' && (*counter) == 0) return BlockComment;
            if (edge == '/') {
                (*counter)--;
                return CommentBody;
            }
            return CommentBody;

        case EscU:
            if (edge == '{') return EscLBr;
            return Error;

        case MltLnStringLit:
            if (edge == '"') return FirstQuote;
            return MltLnStringLit;

        case EndStringLit:
            return Error;

        //##############################
        // Fourth level states
        // BlockComment, EscLBr, FirstQuote
        //##############################
        case BlockComment:
            return Error;

        case EscLBr:
            if (isxdigit(edge)) return FirstHex;
            return Error;

        case FirstQuote:
            if (edge == '"') return SecondQuote;
            return MltLnStringLit;

        //##############################
        // Fifth level states
        // FirstHex, SecondQuote
        //##############################
        case FirstHex:
            if (isxdigit(edge)) return SecondHex;
            return Error;

        case SecondQuote:
            if (edge == '"') return EndMltLnStringLit;
            return MltLnStringLit;

        //##############################
        // Sixth level states
        // SecondHex, EndMltLnStringLit
        //##############################
        case SecondHex:
            if (edge == '}') return StringLit;
            return Error;

        case EndMltLnStringLit:
            return Error;

        default:
            return Error;
    }
}
