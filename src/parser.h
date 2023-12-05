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
    SymtableInit(table); \
    SymtableStackPush(stack, table); \

void StartParser(bool is_first_analysis, symtable_stack_t *stack);
bool Prog(Lexeme *token, symtable_stack_t *stack, bool is_first_analysis);
bool Sequence(Lexeme *token, symtable_stack_t *stack);
bool SequenceN(Lexeme *token, symtable_stack_t *stack);
bool VarDef(Lexeme *token, symtable_stack_t *stack, bool *is_let);
bool VarTypeOrAssign(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item);
bool AssignVar(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item);
bool DefFunction(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token, bool is_first_analysis);
bool VoidF(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token);
bool FirstParamDef(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token);
bool ParamsDef(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token);
bool ParamsNameDef(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token, bool first_or_second, symtable_item_t **param_id_item);
bool ParamsDefN(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token);
bool ReturnFunction(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token);
bool ReturnFunctionN(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token);
bool AssignOrFunction(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item, Lexeme *temp_token);
bool ExpOrCall(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item, bool type_was_defined);
bool CallFunction(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item, symtable_item_t *item_to_assign);
bool FirstParam(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token);
bool ParamsN(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item);
bool Params(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item);
bool ParamsName(Lexeme *token, symtable_stack_t *stack, symtable_item_t *function, Lexeme *param_name_or_id);
bool ElseStat(Lexeme *token, symtable_stack_t *stack);
bool IfExp(Lexeme *token, symtable_stack_t *stack, symtable_item_t **variable);
bool Type(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item, bool param, symtable_item_t *function);
bool IdOrLit(Lexeme *token, symtable_stack_t *stack, symtable_item_t *function);

bool TypeCheck(symtable_item_t *item1, symtable_item_t *item2, int param_index, bool param_handle);
bool FuncReturnTypeCheck(data_type_t return_expression_type, data_type_t function_type);
bool WriteFunc(Lexeme *token, symtable_stack_t *stack, int *counter);

//TODO: find out how to call precendencnu analyzu
bool Expression(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item, bool is_while_or_if, bool is_return, bool type_was_defined);

#endif


