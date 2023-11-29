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
    fprintf(stderr, "Error: parser.c - error code %d at line %d, with token: %d\n", exit_code, token->line, token->kind); \
    exit(exit_code);

#define CREATE_FRAME() \
    Symtable *table = malloc(sizeof(Symtable)); \
    if (table == NULL) { \
        fprintf(stderr, "Error: parser.c - CREATE_FRAME() - malloc failed\n"); \
        exit(INTERNAL_ERROR); \
    } \
    SymtableInit(table);

void StartParser();
bool Prog(Lexeme *token, symtable_stack_t *stack);
bool Sequence(Lexeme *token, symtable_stack_t *stack);
bool SequenceN(Lexeme *token, symtable_stack_t *stack);
bool VarDef(Lexeme *token, symtable_stack_t *stack);
bool VarTypeOrAssign(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token);
bool AssignVar(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token);
bool DefFunction(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token);
bool VoidF(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token);
bool FirstParamDef(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token);
bool ParamsDef(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token);
bool ParamsNameDef(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token, bool first_or_second);
bool ParamsDefN(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token);
bool ReturnFunction(Lexeme *token, symtable_stack_t *stack);
bool ReturnFunctionN(Lexeme *token, symtable_stack_t *stack);
bool AssignOrFunction(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token);
bool ExpOrCall(Lexeme *token, symtable_stack_t *stack);
bool CallFunction(Lexeme *token, symtable_stack_t *stack);
bool FirstParam(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token);
bool ParamsN(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token);
bool Params(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token);
bool ParamsName(Lexeme *token, symtable_stack_t *stack,Lexeme *temp_token, Lexeme *param_name_or_id);
bool ElseStat(Lexeme *token, symtable_stack_t *stack);
bool IfExp(Lexeme *token, symtable_stack_t *stack);
bool Type(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token, bool param);
bool IdOrLit(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token);


//TODO: find out how to call precendencnu analyzu
bool Expression(Lexeme *token, symtable_stack_t *stack);

#endif


