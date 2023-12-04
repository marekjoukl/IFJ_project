#include "automat.h"

AutomatState transition(AutomatState current, char edge)
{
    static unsigned int counter;   // helper variable for counting nested multiline comments
    switch (current)
    {
        //##############################
        // First level states
        // Error, Start, Ls, Gt, ExlPoint, Assign, Comma, Colon, RPar, LPar, SingleQmark, Id, BeginString, IntLit, Plus, Minus, Asterisk, Slash, Space, NewLine, LBrac, RBrac, EndOfFile
        //##############################
        case Error:
            fprintf(stderr, "Error: automat.c - should have created a token already");
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
            if (edge == '{') return LBrac;
            if (edge == '}') return RBrac;
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
            if (edge == EOF) return EndOfFile;
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

        case Id:
            if (edge == '?') return IdNil;
            if (isalnum(edge) || edge == '_') return Id;
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
            if (edge == '>') return Arrow;
            return Error;

        case Asterisk:
            return Error;

        case Slash:
            if (edge == '/') return Comment;
            if (edge == '*') {
                (counter) = 0;
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
        // LsEqual, GtEqual, NotEqual, Equal, DoubleQmark, EmptyString, StringLit, DecPoint, Exp, Comment, CommentBody, EscSeq, IdNil, Arrow
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

        case EmptyString:
            if (edge == '"') return MltLnStringStart;
            return Error;

        case StringLit:
            if (edge == '"') return EndStringLit;
            if (edge == '\\') return EscSeq;
            if (edge == '\n') return Error;
            return StringLit;

        case DecPoint:
            if (isdigit(edge)) return DoubleLitDec;
            return Error;

        case Exp:
            if (isdigit(edge)) return DoubleLitExp;
            if (edge == '+' || edge == '-') return ExpSign;
            return Error;

        case Comment:
            if (edge != '\n' && edge != '\r' && edge != EOF) return Comment;
            return Error;

        case CommentBody:
            if (edge == '/') return NestedComment;
            if (edge == '*') return CommentEnding;
            if (edge == EOF) return Error;
            return CommentBody;

        case EscSeq:
            if (edge == 'u') return EscU;
            if (edge == 'n' || edge == 'r' || edge == 't' || edge == '"' || edge == '\\') return StringLit;
            return Error;

        case IdNil:
            return Error;

        case Arrow:
            return Error;

        //##############################
        // Third level states
        // DoubleLitDec, DoubleLitExp, ExpSign, NestedComment, CommentEnding, EscU, EndStringLit, MltLnStringStart
        //##############################
        case DoubleLitDec:
            if (isdigit(edge)) return DoubleLitDec;
            if (edge == 'e' || edge == 'E') return Exp;
            return Error;

        case DoubleLitExp:
            if (isdigit(edge)) return DoubleLitExp;
            return Error;

        case ExpSign:
            if (isdigit(edge)) return DoubleLitExp;
            return Error;

        case NestedComment:
            if (edge == '*') {
                (counter)++;
                return CommentBody;
            }
            if (edge == EOF) return Error;
            return CommentBody;

        case CommentEnding:
            if (edge == '/' && (counter) == 0) return BlockComment;
            if (edge == '/') {
                (counter)--;
                return CommentBody;
            }
            if (edge == EOF) return Error;
            return CommentBody;

        case EscU:
            if (edge == '{') return EscLBr;
            return Error;

        case MltLnStringStart:
            if (edge == '\n') return MltLnStringLit;
            return Error;

        case EndStringLit:
            return Error;

        //##############################
        // Fourth level states
        // BlockComment, EscLBr, MltLnStringLit
        //##############################
        case BlockComment:
            return Error;

        case EscLBr:
            if (isxdigit(edge)) return FirstHex;
            return Error;

        case MltLnStringLit:
            if (edge == '"') return FirstQuoteErr;
            if (edge == '\n') return MltLnStringStartEnd;
            return MltLnStringLit;

        //##############################
        // Fifth level states
        // FirstHex, MltLnStringStartEnd, FirstQuoteErr
        //##############################
        case FirstHex:
            if (isxdigit(edge)) return SecondHex;
            return Error;

        case MltLnStringStartEnd:
            if (edge == '"') return FirstQuote;
            if (edge == '\n') return MltLnStringStartEnd;
            if ((edge != '"') && (edge != '\n')) return MltLnStringLit;
            return Error;

        case FirstQuoteErr:
            if (edge == '"') return SecondQuoteErr;
            return MltLnStringLit;

        //##############################
        // Sixth level states
        // SecondHex, FirstQuote
        //##############################
        case SecondHex:
            if (edge == '}') return StringLit;
            return Error;

        case FirstQuote:
            if (edge == '"') return SecondQuote;
            return MltLnStringLit;

        case SecondQuoteErr:
            if (edge == '"') return ThirdQuoteErr;
            return MltLnStringLit;

        //##############################
        // Seventh level states
        // SecondQuote, ThirdQuoteErr, 
        //##############################
        case SecondQuote:
            if (edge == '"') return EndMltLnStringLit;
            return MltLnStringLit;

        case ThirdQuoteErr:
            return Error;

        //##############################
        // Eighth level states
        // EndMltLnStringLit
        //##############################
        case EndMltLnStringLit:
            return Error;

        default:
            return Error;
    }
}
