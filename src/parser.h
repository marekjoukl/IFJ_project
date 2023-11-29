/**
 * @file parser.h
 * @author Marek Joukl (xjoukl00), Ondrej Kožányi (xkozan01)
 * @brief Header for parser
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "scanner.h"
#include "precedent.h"
#include "error.h"
#include "symtable.h"

#define GETTOKEN() \
    *token = get_next_non_whitespace_lexeme();

#define ERROR_HANDLE(exit_code, token) \
    fprintf(stderr, "Error: parser.c - syntax error at line %d, with token: %d\n", token->line, token->kind); \
    exit(exit_code);

bool Prog(Lexeme *token);
bool Sequence(Lexeme *token);
bool SequenceN(Lexeme *token);
bool VarDef(Lexeme *token);
bool VarTypeOrAssign(Lexeme *token);
bool AssignVar(Lexeme *token);
bool DefFunction(Lexeme *token);
bool VoidF(Lexeme *token);
bool FirstParamDef(Lexeme *token);
bool ParamsDef(Lexeme *token);
bool ParamsNameDef(Lexeme *token);
bool ParamsDefN(Lexeme *token);
bool ReturnFunction(Lexeme *token);
bool ReturnFunctionN(Lexeme *token);
bool AssignOrFunction(Lexeme *token);
bool ExpOrCall(Lexeme *token);
bool CallFunction(Lexeme *token);
bool FirstParam(Lexeme *token);
bool ParamsN(Lexeme *token);
bool Params(Lexeme *token);
bool ParamsName(Lexeme *token);
bool ElseStat(Lexeme *token);
bool IfExp(Lexeme *token);
bool Type(Lexeme *token);
bool IdOrLit(Lexeme *token);


//TODO: find out how to call precendencnu analyzu
bool Expression(Lexeme *token);

#endif


