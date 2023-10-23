#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum AutomatState
{
    Error,
    Start,
    Ls,
    LsEqual,
    Gt,
    GtEqual,
    ExlPoint,
    NotEqual,
    Assign,
    Equal,
    Comma,
    Colon,
    RPar,
    LPar,
    SingleQmark,
    DoubleQmark,
    Id,
    IdTypeNil,
    BeginString,
    StringLit,
    EscSeq,
    EscU,
    EscLBr,
    FirstHex,
    SecondHex,
    EndStringLit,
    EmptyString,
    MltLnStringLit,
    FirstQuote,
    SecondQuote,
    EndMltLnStringLit,
    IntLit,
    DecPoint,
    DoubleLit,
    Exp,
    ExpSign,
    Plus,
    Minus,
    Asterisk,
    Slash,
    Comment,
    CommentBody,
    NestedComment,
    CommentEnding,
    BlockComment,
    Space,
    NewLine,
} AutomatState;

AutomatState transition(AutomatState current, char edge, unsigned int *counter);