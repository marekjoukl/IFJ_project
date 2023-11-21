/**
 * @file parser.h
 * @author Marek Joukl (xjoukl00)
 * @brief Header for parser
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "scanner.h"

bool Prog(Lexeme token);
bool Sequence(Lexeme token);
bool VarDef(Lexeme token);
bool VarTypeDef(Lexeme token);
bool AssignVar(Lexeme token);
bool DefFunction(Lexeme token);
bool FirstParamDef(Lexeme token);
bool ParamsDef(Lexeme token);
bool ParamsNameDef(Lexeme token);
bool ParamsDefN(Lexeme token);
bool ReturnFunction(Lexeme token);
bool AssignOrFunction(Lexeme token);
bool ExpOrCall(Lexeme token);
bool CallFunction(Lexeme token);
bool FirstParam(Lexeme token);
bool ParamsN(Lexeme token);
bool Params(Lexeme token);
bool ParamsName(Lexeme token);
bool ElseStat(Lexeme token);
bool IfExp(Lexeme token);
bool Type(Lexeme token);
bool QMark(Lexeme token);
bool IdOrLit(Lexeme token);


//TODO: find out how to call precendencnu analyzu
bool Expression(Lexeme token);

#endif


