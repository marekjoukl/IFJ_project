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
#include "symtable_stack.h"

#define GETTOKEN() \
    *token = get_next_non_whitespace_lexeme();

#define ERROR_HANDLE(exit_code, token) \
    fprintf(stderr, "Error: parser.c - syntax error at line %d, with token: %d\n", token->line, token->kind); \
    exit(exit_code);

void StartParser();
bool Prog(Lexeme *token, symtable_stack_t *stack);
bool Sequence(Lexeme *token, symtable_stack_t *stack);
bool SequenceN(Lexeme *token, symtable_stack_t *stack);
bool VarDef(Lexeme *token, symtable_stack_t *stack);
bool VarTypeOrAssign(Lexeme *token, symtable_stack_t *stack);
bool AssignVar(Lexeme *token, symtable_stack_t *stack);
bool DefFunction(Lexeme *token, symtable_stack_t *stack);
bool VoidF(Lexeme *token, symtable_stack_t *stack);
bool FirstParamDef(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token);
bool ParamsDef(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token);
bool ParamsNameDef(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token, bool first_or_second);
bool ParamsDefN(Lexeme *token, symtable_stack_t *stack);
bool ReturnFunction(Lexeme *token, symtable_stack_t *stack);
bool ReturnFunctionN(Lexeme *token, symtable_stack_t *stack);
bool AssignOrFunction(Lexeme *token, symtable_stack_t *stack);
bool ExpOrCall(Lexeme *token, symtable_stack_t *stack);
bool CallFunction(Lexeme *token, symtable_stack_t *stack);
bool FirstParam(Lexeme *token, symtable_stack_t *stack);
bool ParamsN(Lexeme *token, symtable_stack_t *stack);
bool Params(Lexeme *token, symtable_stack_t *stack);
bool ParamsName(Lexeme *token, symtable_stack_t *stack);
bool ElseStat(Lexeme *token, symtable_stack_t *stack);
bool IfExp(Lexeme *token, symtable_stack_t *stack);
bool Type(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token);
bool IdOrLit(Lexeme *token, symtable_stack_t *stack);


//TODO: find out how to call precendencnu analyzu
bool Expression(Lexeme *token, symtable_stack_t *stack);

#endif


