/**
 * @file parser.c
 * @author Marek Joukl (xjoukl00), Ondrej Kožányi (xkozan01)
 * @brief Parser using recursive descent
 */

#include "parser.h"

data_t *CreateData(bool function, int line) {
    data_t *data = malloc(sizeof(data_t));
    if (data == NULL) {
        fprintf(stderr, "Error: parser.c - data_init() - malloc failed\n");
        exit(INTERNAL_ERROR);
    }
    data->is_function = function;
    data->item_type = TYPE_UNDEFINED;
    data->can_be_nil = false;
    data->line = line;
    data->value_is_nil = false;
    data->param_count = 0;
    data->param_names = NULL;
    data->params_id = NULL;
    data->param_types = NULL;
    data->is_modifiable = true;
    //data->is_defined = true;
    data->string_value = NULL;
    data->numeric_value = 0;
    data->blinded_sign = false;
    data->param_count_current = 0;
    return data;
}

void StartParser() {
    symtable_stack_t *stack = SymtableStackInit();
    Symtable *table = malloc(sizeof(Symtable));
    if (table == NULL) {
        fprintf(stderr, "Error: parser.c - StartParser() - malloc failed\n");
        exit(INTERNAL_ERROR);
    }
    Lexeme token;
    SymtableInit(table);
    SymtableStackPush(stack, table);

    token = get_next_non_whitespace_lexeme();

    Prog(&token, stack);

    SymtableStackPop(stack);
    SymtableStackDispose(stack);
}

bool Prog(Lexeme *token, symtable_stack_t *stack) {
    symtable_item_t *item = NULL;
    // <PROG> -> LEX_EOF
    if (token->kind == LEX_EOF) return true;

    // <PROG> -> <SEQUENCE> <PROG>
    if (token->kind == IDENTIFIER || token->kind == IF || token->kind == LET || token->kind == WHILE || token->kind == VAR) {
        if (!Sequence(token, stack)) {
            ERROR_HANDLE(SYNTAX_ERROR, token);
        }

        if (!Prog(token, stack)) {
            ERROR_HANDLE(SYNTAX_ERROR, token);
        }

        return true;
    }

    // <PROG> -> FUNC IDENTIFIER LEFT_PAR <FIRST_PARAM_DEF> <DEF_FUNCTION> <PROG>
    if (token->kind == FUNC) {
        GETTOKEN()
        if (token->kind != IDENTIFIER)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        item = SymtableSearchAll(stack, token->extra_data.string);
        if (item != NULL) {
            ERROR_HANDLE(DEFINITION_ERROR, token); //TODO: find out what error code to use
        }

        data_t *data = CreateData(true, token->line);
        SymtableAddItem(stack->array[stack->size - 1], token->extra_data.string, data);
        item = SymtableSearchAll(stack, token->extra_data.string);

        CREATE_FRAME()

        GETTOKEN()

        if (token->kind != LEFT_PAR) {
            ERROR_HANDLE(SYNTAX_ERROR, token);
        }
        GETTOKEN()

        if (!FirstParamDef(token, stack, item)) {
            ERROR_HANDLE(SYNTAX_ERROR, token);
        }

        if (!DefFunction(token, stack, item)) {
            ERROR_HANDLE(SYNTAX_ERROR, token);
        }

        if (!Prog(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }

    { ERROR_HANDLE(SYNTAX_ERROR, token); }
}

bool Sequence(Lexeme *token, symtable_stack_t *stack) {
    symtable_item_t *item = NULL;

    // <SEQUENCE> -> <VAR_DEF> IDENTIFIER <VAR_TYPE_OR_ASSIGN>
    if (token->kind == LET || token->kind == VAR) {
        bool is_var = false;
        if (!VarDef(token, stack, &is_var))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (token->kind != IDENTIFIER)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        item = SymtableSearch(stack->array[stack->size - 1], token->extra_data.string);
        if (item != NULL) {
            ERROR_HANDLE(DEFINITION_ERROR, token);
        }

        data_t *data = CreateData(false, token->line);
        data->is_modifiable = is_var;

        SymtableAddItem(stack->array[stack->size - 1], token->extra_data.string, data);
        item = SymtableSearchAll(stack, token->extra_data.string);

        GETTOKEN()
        if (!VarTypeOrAssign(token, stack, item))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }

    // <SEQUENCE> -> IDENTIFIER <ASSIGN_OR_FUNCTION>
    else if (token->kind == IDENTIFIER) {
        item = SymtableSearchAll(stack, token->extra_data.string);
        if (item == NULL) {
            ERROR_HANDLE(UNDEFINED_VAR_ERROR, token);
        }

        GETTOKEN()
        if (!AssignOrFunction(token, stack, item))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }

    // <SEQUENCE> -> IF <IF_EXP> LEFT_BRACKET <SEQUENCE_N> RIGHT_BRACKET <ELSE_STAT>
    else if (token->kind == IF) {
        symtable_item_t *variable = NULL;
        GETTOKEN()
        if (!IfExp(token, stack, variable))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (token->kind != LEFT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN()

        //creating a new frame for if clause
        CREATE_FRAME();

        if (variable != NULL) {
            data_t *data = malloc(sizeof(data_t));
            if (data == NULL) {
                ERROR_HANDLE(INTERNAL_ERROR, token)
            }
            *data = *(variable->data);
            data->can_be_nil = false;
            SymtableAddItem(stack->array[stack->size - 1], variable->key, data);
        }

        if (!SequenceN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        //deleting assigned frame
        SymtableStackPop(stack);

        if (token->kind != RIGHT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN()

        if (!ElseStat(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    // <SEQUENCE> -> WHILE <EXPRESSION> LEFT_BRACKET <SEQUENCE_N> RIGHT_BRACKET
    else if (token->kind == WHILE) {
        GETTOKEN()
        if (!Expression(token, stack, NULL, true, false, false))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (token->kind != LEFT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN()

        //creating a new frame for if clause
        CREATE_FRAME();

        if(!SequenceN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        //deleting assigned frame
        SymtableStackPop(stack);

        if(token->kind != RIGHT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN()

        return true;
    }

    //ukoncenie rekurzie
    else if (token->kind == RIGHT_BRACKET || token->kind == LEX_EOF) {
        return true;
    }
    return false;
}

bool SequenceN(Lexeme *token, symtable_stack_t *stack) {
    // <SEQUENCE_N> -> ε
    if (token->kind == RIGHT_BRACKET || token->kind == RETURN) {
        return true;
    }
    // <SEQUENCE_N> -> <SEQUENCE> <SEQUENCE_N>
    if (token->kind == IDENTIFIER || token->kind == IF || token->kind == LET || token->kind == WHILE || token->kind == VAR) {
        if (!Sequence(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (!SequenceN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool AssignOrFunction(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item) {
    //symtable_item_t *variable = SymtableSearchAll(stack, temp_token->extra_data.string);
    // <ASSIGN_OR_FUNCTION> -> ASSIGNMENT <EXP_OR_CALL>
    if (token->kind == ASSIGNMENT) {
        if (item->data->is_function) {
            ERROR_HANDLE(OTHER_SEMANTIC_ERROR, token);
        }
        if (!item->data->is_modifiable) {
            ERROR_HANDLE(OTHER_SEMANTIC_ERROR, token); //TODO: find out what error code to use
        }
        GETTOKEN()
        if (!ExpOrCall(token, stack, item, true))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
    // <ASSIGN_OR_FUNCTION> -> LEFT_PAR <FIRST_PARAM>
    if (token->kind == LEFT_PAR){
        if (!item->data->is_function) {
            ERROR_HANDLE(OTHER_SEMANTIC_ERROR, token);
        }
        GETTOKEN()
        if(!FirstParam(token, stack, item))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
    return false;
}

bool DefFunction(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token) {
    // <DEF_FUNCTION> -> <VOID_F> LEFT_BRACKET <SEQUENCE_N> <RETURN_FUNCTION> RIGHT_BRACKET
    if (token->kind == LEFT_BRACKET || token->kind == ARROW) {
        if (!VoidF(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (token->kind != LEFT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN()

        if (!SequenceN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (!ReturnFunction(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        //deleting assigned frame
        SymtableStackPop(stack);

        if (token->kind != RIGHT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN()

        return true;
    }
    return false;
}

bool VoidF(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token) {
    // <VOID_F> -> ε
    if (token->kind == LEFT_BRACKET) {
        return true;
    }
    // <VOID_F> -> ARROW <TYPE>
    if (token->kind == ARROW) {
        GETTOKEN()
        if (!Type(token, stack, temp_token, false, NULL))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
    return false;
}

bool ReturnFunction(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token) {
    // <RETURN_FUNCTION> -> RETURN <RETURN_FUNCTION_N>
    if (token->kind == RETURN){
        GETTOKEN()
        if (!ReturnFunctionN(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    // <RETURN_FUNCTION> -> ε
    if (token->kind == RIGHT_BRACKET) {
        if (temp_token->data->item_type != TYPE_UNDEFINED) {
            ERROR_HANDLE(RETURN_ERROR, token);
        }
        return true;
    }

    return false;
}

bool ReturnFunctionN(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token) {
    // <RETURN_FUNCTION_N> -> ε
    if (token->kind == RIGHT_BRACKET) {
        if (temp_token->data->item_type != TYPE_UNDEFINED) {
            ERROR_HANDLE(RETURN_ERROR, token);
        }
        return true;
    }

    // <RETURN_FUNCTION_N> -> <EXPRESSION>
    if (token->kind == IDENTIFIER || token->kind == INTEGER_LIT || token->kind == DOUBLE_LIT || token->kind == STRING_LIT || token->kind == LEFT_PAR) {
        if (!Expression(token, stack, temp_token, false, true, true))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
    return false;
}

bool  FirstParamDef(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token) {
    // <FIRST_PARAM_DEF> -> RIGHT_PAR
    if (token->kind == RIGHT_PAR) {
        GETTOKEN()
        return true;
    }
    // <FIRST_PARAM_DEF> -> <PARAMS_DEF> <PARAMS_DEF_N>
    if (token->kind == IDENTIFIER || token->kind == UNDERSCORE) {
        if (!ParamsDef(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if(!ParamsDefN(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool ParamsDef(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token) {
    symtable_item_t *param_id_item = NULL;
    bool first_or_second = true;
    // <PARAMS_DEF> -> <PARAMS_NAME_DEF> <PARAMS_NAME_DEF> COLON <TYPE>
    if (token->kind == UNDERSCORE || token->kind == IDENTIFIER) {
        if (!ParamsNameDef(token, stack, temp_token, first_or_second, &param_id_item))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        first_or_second = false;

        if (token->kind == IDENTIFIER || token->kind == UNDERSCORE) {
            if (!ParamsNameDef(token, stack, temp_token, first_or_second, &param_id_item))
                { ERROR_HANDLE(SYNTAX_ERROR, token); }
        }
        else { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (token->kind != COLON)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        GETTOKEN()
        if (!Type(token, stack, param_id_item, true, temp_token)) //TODO: finish semantics in TYPE
            { ERROR_HANDLE(TYPE_DEDUCTION_ERROR, token); }

        return true;
    }
    return false;
}

bool ParamsNameDef(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token, bool first_or_second, symtable_item_t **param_id_item) {
    //symtable_item_t *item = SymtableSearchAll(stack, temp_token->extra_data.string);
    if (first_or_second) {
        temp_token->data->param_count++;
        temp_token->data->param_names = realloc(temp_token->data->param_names, sizeof(char *) * temp_token->data->param_count);
    }
    else temp_token->data->params_id = realloc(temp_token->data->params_id, sizeof(char*) * temp_token->data->param_count);

    // <PARAMS_NAME_DEF> -> UNDERSCORE
    if (token->kind == UNDERSCORE) {
        if (first_or_second) temp_token->data->param_names[temp_token->data->param_count - 1] = NULL;
        else temp_token->data->params_id[temp_token->data->param_count - 1] = NULL;
        GETTOKEN()
        return true;
    }
    // <PARAMS_NAME_DEF> -> IDENTIFIER
    if (token->kind == IDENTIFIER) {
        if (first_or_second) temp_token->data->param_names[temp_token->data->param_count - 1] = token->extra_data.string;
        else {
            temp_token->data->params_id[temp_token->data->param_count - 1] = token->extra_data.string;
            *param_id_item = SymtableSearch(stack->array[stack->size - 1], token->extra_data.string);
            if (*param_id_item != NULL) {
                ERROR_HANDLE(DEFINITION_ERROR, token);
            }
            data_t *data = CreateData(false, token->line);
            SymtableAddItem(stack->array[stack->size - 1], token->extra_data.string, data);
            *param_id_item = SymtableSearchAll(stack, token->extra_data.string);
        }
        GETTOKEN()
        return true;
    }
    return false;
}

bool ParamsDefN(Lexeme *token, symtable_stack_t *stack, symtable_item_t *temp_token) {
    // <PARAMS_DEF_N> -> RIGHT_PAR
    if (token->kind == RIGHT_PAR) {
        GETTOKEN()
        return true;
    }
    // <PARAMS_DEF_N> -> COMMA <PARAMS_DEF> <PARAMS_DEF_N>
    if (token->kind == COMMA){
        GETTOKEN()
        if (!ParamsDef(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if(!ParamsDefN(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool FirstParam(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item){
    //symtable_item_t *item = SymtableSearchAll(stack, temp_token->extra_data.string);
    item->data->param_count_current = 0;
    // <FIRST_PARAM> -> RIGHT_PAR
    if (token->kind == RIGHT_PAR) {
        if (item->data->param_count != 0) {     //TODO: what about function(_ _) ?
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token); //TODO: find out what error code to use
        }
        GETTOKEN()
        return true;
    }
    // <FIRST_PARAM> -> <PARAMS> <PARAMS_N>
    if (token->kind == INTEGER_LIT || token->kind == IDENTIFIER || token->kind == STRING_LIT || token->kind == DOUBLE_LIT) {
        if (!Params(token, stack, item))
        { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if(!ParamsN(token, stack, item))
        { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
    return false;
}

bool ParamsN(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item) {
    // <PARAMS_N> -> RIGHT_PAR
    if (token->kind == RIGHT_PAR) {
        if (item->data->param_count_current != item->data->param_count) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        GETTOKEN()
        return true;
    }

    // <PARAMS_N> -> COMMA <PARAMS> <PARAMS_N>
    if (token->kind == COMMA){
        GETTOKEN()
        if (!Params(token, stack, item))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if(!ParamsN(token, stack, item))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool Params(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item) {
    Lexeme param_id;
    if (item->data->param_count_current >= item->data->param_count) {
        ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
    }
    //symtable_item_t *item = SymtableSearchAll(stack, temp_token->extra_data.string);
    //  <PARAMS> -> INT_LIT
    if (token->kind == INTEGER_LIT) {
        if (item->data->param_names[item->data->param_count_current] != NULL) {
            ERROR_HANDLE(OTHER_SEMANTIC_ERROR, token);
        }
        if (item->data->param_types[item->data->param_count_current] != TYPE_INT && item->data->param_types[item->data->param_count_current] != TYPE_INT_NIL) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        item->data->param_count_current++;
        GETTOKEN()
        return true;
    }
    //  <PARAMS> -> STRING_LIT
    if (token->kind == STRING_LIT) {
        if (item->data->param_names[item->data->param_count_current] != NULL) {
            ERROR_HANDLE(OTHER_SEMANTIC_ERROR, token);
        }
        if (item->data->param_types[item->data->param_count_current] != TYPE_STRING && item->data->param_types[item->data->param_count_current] != TYPE_STRING_NIL) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        item->data->param_count_current++;
        GETTOKEN()
        return true;
    }
    //  <PARAMS> -> DOUBLE_LIT
    if (token->kind == DOUBLE_LIT) {
        if (item->data->param_names[item->data->param_count_current] != NULL) {
            ERROR_HANDLE(OTHER_SEMANTIC_ERROR, token);
        }
        if (item->data->param_types[item->data->param_count_current] != TYPE_DOUBLE && item->data->param_types[item->data->param_count_current] != TYPE_DOUBLE_NIL) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        item->data->param_count_current++;
        GETTOKEN()
        return true;
    }
    // <PARAMS> -> IDENTIFIER <PARAMS_NAME>
    if (token->kind == IDENTIFIER){
        param_id = *token;
        GETTOKEN()
        if(!ParamsName(token, stack, item, &param_id))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        item->data->param_count_current++;
        return true;
    }
    // <PARAMS> -> NIL
    if (token->kind == NIL) { //TODO: finish semantics in NIL case
        if (item->data->param_names[item->data->param_count_current] != NULL) {
            ERROR_HANDLE(OTHER_SEMANTIC_ERROR, token);
        }
        data_type_t type = item->data->param_types[item->data->param_count_current];
        if (type != TYPE_INT_NIL && type != TYPE_DOUBLE_NIL && type != TYPE_STRING_NIL) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        item->data->param_count_current++;
        GETTOKEN()
        return true;
    }

    return false;
}

bool ParamsName(Lexeme *token, symtable_stack_t *stack, symtable_item_t *function, Lexeme *param_name_or_id) {
    //symtable_item_t *function = SymtableSearchAll(stack, temp_token->extra_data.string);
    // <PARAMS_NAME> -> COLON <ID_OR_LIT>
    if (token->kind == COLON){
        if (strcmp(param_name_or_id->extra_data.string, function->data->param_names[function->data->param_count_current]) != 0) {
            ERROR_HANDLE(OTHER_SEMANTIC_ERROR, token);
        }
        GETTOKEN()
        if(!IdOrLit(token, stack, function))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    // <PARAMS_NAME> -> ε
    if (token->kind == COMMA || token->kind == RIGHT_PAR) {
        symtable_item_t *param_item = SymtableSearchAll(stack, param_name_or_id->extra_data.string);
        if (param_item == NULL) {
            ERROR_HANDLE(UNDEFINED_VAR_ERROR, token);
        }
        if (function->data->param_names[function->data->param_count_current] != NULL) {
            ERROR_HANDLE(OTHER_SEMANTIC_ERROR, token);
        }
        if (!TypeCheck(function, param_item, function->data->param_count_current, true)) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        return true;
    }

    return false;
}

bool IdOrLit(Lexeme *token, symtable_stack_t *stack, symtable_item_t *function) {
    // <ID_OR_LIT> -> IDENTIFIER || INT_LIT || DOUBLE_LIT || STRING_lIT || NIL
    if (token->kind == IDENTIFIER) {
        symtable_item_t *param = SymtableSearchAll(stack, token->extra_data.string);
        if (param == NULL) {
            ERROR_HANDLE(UNDEFINED_VAR_ERROR, token);
        }
        if (!TypeCheck(function, param, function->data->param_count_current, true)) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        GETTOKEN()
        return true;
    }
    if (token->kind == STRING_LIT) {
        if (function->data->param_types[function->data->param_count_current] != TYPE_STRING && function->data->param_types[function->data->param_count_current] != TYPE_STRING_NIL) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        GETTOKEN()
        return true;
    }
    if (token->kind == DOUBLE_LIT) {
        if (function->data->param_types[function->data->param_count_current] != TYPE_DOUBLE && function->data->param_types[function->data->param_count_current] != TYPE_DOUBLE_NIL) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        GETTOKEN()
        return true;
    }
    if (token->kind ==  INTEGER_LIT) {
        if (function->data->param_types[function->data->param_count_current] != TYPE_INT && function->data->param_types[function->data->param_count_current] != TYPE_INT_NIL) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        GETTOKEN()
        return true;
    }
    if (token->kind == NIL) {
        data_type_t type = function->data->param_types[function->data->param_count_current];
        if (type != TYPE_INT_NIL && type != TYPE_DOUBLE_NIL && type != TYPE_STRING_NIL) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        GETTOKEN()
        return true;
    }
    return false;
}

bool IfExp(Lexeme *token, symtable_stack_t *stack, symtable_item_t *variable) {
    // <IF_EXP> -> LET IDENTIFIER
    if (token->kind == LET) {
        GETTOKEN()
        if (token->kind == IDENTIFIER) {
            variable = SymtableSearchAll(stack, token->extra_data.string);
            if (variable == NULL) {
                ERROR_HANDLE(UNDEFINED_VAR_ERROR, token);
            }
            if (variable->data->is_modifiable) {
                ERROR_HANDLE(OTHER_SEMANTIC_ERROR, token); //TODO: find out what error code to use
            }
            GETTOKEN()
            return true;
        }
        return false;
    }
    // <IF_EXP> -> <EXPRESSION>
    if (token->kind == IDENTIFIER || token->kind == INTEGER_LIT || token->kind == DOUBLE_LIT || token->kind == STRING_LIT || token->kind == LEFT_PAR) {
        if (!Expression(token, stack, NULL, true, false, false))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool ElseStat(Lexeme *token, symtable_stack_t *stack) {
    // <ELSE_STAT> -> ELSE LEFT_BRACKET <SEQUENCE_N> RIGHT_BRACKET
    if (token->kind == ELSE) {
        GETTOKEN()
        if (token->kind != LEFT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN()

        //creating a new frame for else clause
        CREATE_FRAME();

        if (!SequenceN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        //deleting assigned frame
        SymtableStackPop(stack);

        if (token->kind != RIGHT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN()

        return true;
    }

    return false;
}

bool VarDef(Lexeme *token, symtable_stack_t *stack, bool *is_var) {
    // <VAR_DEF> -> LET
    if (token->kind == LET) {
        GETTOKEN()
        return true;
    }

    // <VAR_DEF> -> VAR
    if (token->kind == VAR) {
        *is_var = true;
        GETTOKEN()
        return true;
    }

    return false;
}

bool VarTypeOrAssign(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item) {
    bool type_was_defined = false;
    // <VAR_TYPE_OR_ASSIGN> -> COLON <TYPE> <ASSIGN_VAR>
    if (token->kind == COLON) {
        GETTOKEN()
        if (!Type(token, stack, item, false, NULL))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (!AssignVar(token, stack, item))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    // <VAR_TYPE_OR_ASSIGN> -> ASSIGNMENT <EXP_OR_CALL>
    else if (token->kind == ASSIGNMENT) {
        GETTOKEN()
        if (!ExpOrCall(token, stack, item, false))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool AssignVar(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item) {
    // <ASSIGN_VAR> -> ASSIGNMENT <EXP_OR_CALL>
    if (token->kind == ASSIGNMENT) {
        GETTOKEN()
        if (!ExpOrCall(token, stack, item, true))
        { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
        // <ASSIGN_VAR> -> ε
    else if (token->kind == IDENTIFIER || token->kind == LEX_EOF || token->kind == FUNC || token->kind == IF || token->kind == LET || token->kind == WHILE || token->kind == VAR || token->kind == RIGHT_BRACKET || token->kind == RETURN) {
        return true;
    }
    return false;
}

bool Type(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item, bool param, symtable_item_t *function) {
    //symtable_item_t *item = SymtableSearchAll(stack, temp_token->extra_data.string);
    if (param) {
        function->data->param_types = realloc(function->data->param_types, sizeof(data_type_t) * function->data->param_count);
    }
    // <TYPE> -> INT
    if (token->kind == INT) {
        if (param) {
            item->data->item_type = TYPE_INT;
            if (token->nil_type) {
                function->data->param_types[function->data->param_count - 1] = TYPE_INT_NIL;
                item->data->can_be_nil = true;
            }
            else {
                function->data->param_types[function->data->param_count - 1] = TYPE_INT;
            }
        }
        else if (item->data->is_function) {
            if (token->nil_type) {
                item->data->item_type = TYPE_INT_NIL;
            }
            else {
                item->data->item_type = TYPE_INT;
            }
            item->data->can_be_nil = token->nil_type;
        }
        else {
            item->data->item_type = TYPE_INT;
            item->data->can_be_nil = token->nil_type;
        }
        GETTOKEN()
        return true;
    }
    // <TYPE> -> STRING
    else if (token->kind == STRING) {
        if (param) {
            item->data->item_type = TYPE_STRING;
            if (token->nil_type) {
                function->data->param_types[function->data->param_count - 1] = TYPE_STRING_NIL;
                item->data->can_be_nil = true;
            }
            else {
                function->data->param_types[function->data->param_count - 1] = TYPE_STRING;
            }
        }
        else if (item->data->is_function) {
            if (token->nil_type) {
                item->data->item_type = TYPE_STRING_NIL;
            }
            else {
                item->data->item_type = TYPE_STRING;
            }
            item->data->can_be_nil = token->nil_type;
        }
        else {
            item->data->item_type = TYPE_STRING;
            item->data->can_be_nil = token->nil_type;
        }
        GETTOKEN()
        return true;
    }
    // <TYPE> -> DOUBLE
    else if (token->kind == DOUBLE) {
        if (param) {
            item->data->item_type = TYPE_DOUBLE;
            if (token->nil_type) {
                function->data->param_types[function->data->param_count - 1] = TYPE_DOUBLE_NIL;
                item->data->can_be_nil = true;
            }
            else {
                function->data->param_types[function->data->param_count - 1] = TYPE_DOUBLE;
            }
        }
        else if (item->data->is_function) {
            if (token->nil_type) {
                item->data->item_type = TYPE_DOUBLE_NIL;
            }
            else {
                item->data->item_type = TYPE_DOUBLE;
            }
            item->data->can_be_nil = token->nil_type;
        }
        else {
            item->data->item_type = TYPE_DOUBLE;
            item->data->can_be_nil = token->nil_type;
        }
        GETTOKEN()
        return true;
    }
    return false;
}

bool ExpOrCall(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item_to_assign, bool type_was_defined) {
    symtable_item_t *item = NULL;
    if (token->kind == IDENTIFIER) {
        item = SymtableSearchAll(stack, token->extra_data.string);
        if (item == NULL) {
            GETTOKEN()
            if (token->kind == LEFT_PAR)
                { ERROR_HANDLE(DEFINITION_ERROR, token); }
            else
                { ERROR_HANDLE(UNDEFINED_VAR_ERROR, token); }
        }
    }

    // <EXP_OR_CALL> -> <CALL_FUNCTION>
    if (token->kind == IDENTIFIER && item->data->is_function) {
        if (type_was_defined) {
            if (!TypeCheck(item, item_to_assign, 0, false)) {
                ERROR_HANDLE(TYPE_ERROR, token);
            }
        }
        else {
            switch (item->data->item_type) {
                case TYPE_INT:
                    item_to_assign->data->item_type = TYPE_INT;
                    break;

                case TYPE_DOUBLE:
                    item_to_assign->data->item_type = TYPE_DOUBLE;
                    break;

                case TYPE_STRING:
                    item_to_assign->data->item_type = TYPE_STRING;
                    break;

                case TYPE_INT_NIL:
                    item_to_assign->data->item_type = TYPE_INT;
                    item_to_assign->data->can_be_nil = true;
                    break;

                case TYPE_DOUBLE_NIL:
                    item_to_assign->data->item_type = TYPE_DOUBLE;
                    item_to_assign->data->can_be_nil = true;
                    break;

                case TYPE_STRING_NIL:
                    item_to_assign->data->item_type = TYPE_STRING;
                    item_to_assign->data->can_be_nil = true;
                    break;

                default:
                    ERROR_HANDLE(TYPE_ERROR, token);
            }
        }
        if (!CallFunction(token, stack, item))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }

    // <EXP_OR_CALL> -> <EXPRESSION>
    else if (token->kind == IDENTIFIER || token->kind == INTEGER_LIT || token->kind == DOUBLE_LIT || token->kind == STRING_LIT || token->kind == LEFT_PAR || token->kind == NIL) {
        if (!Expression(token, stack, item_to_assign, false, false, type_was_defined))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }

    return false;
}

bool CallFunction(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item) {
    // <CALL_FUNCTION> -> IDENTIFIER LEFT_PAR <FIRST_PARAM>
    if (token->kind == IDENTIFIER) {
        GETTOKEN()
        if (token->kind != LEFT_PAR)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN()

        if (!FirstParam(token, stack, item))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool Expression(Lexeme *token, symtable_stack_t *stack, symtable_item_t *item, bool is_while_or_if, bool is_return, bool type_was_defined) {
    data_type_t expression_type;
    //TODO: pridat stack do precedencnej + poslat tam item funkcie/premennej na kontrolu tipov pri return funkcie/priradenie do premennej
    expression_type = precedent_analysys(token, stack);
    if (is_while_or_if) {
        if (expression_type != TYPE_BOOL) {
            ERROR_HANDLE(TYPE_ERROR, token); //TODO: find out what error code to use
        }
    }
    else if (type_was_defined) {
        if (expression_type == TYPE_BOOL) {
            ERROR_HANDLE(TYPE_ERROR, token); //TODO: find out what error code to use
        }
        else if (is_return) {
            if (!FuncReturnTypeCheck(expression_type, item->data->item_type)) {
                ERROR_HANDLE(TYPE_ERROR, token); //TODO: find out what error code to use
            }
        }
        else if (expression_type == TYPE_NIL) {
            if (item->data->can_be_nil) {
                return true;
            }
        }
        else if (expression_type != item->data->item_type) { //TODO: moze sa stat ze sa mi vrati typ napr TYPE_INT_NIL?
            ERROR_HANDLE(TYPE_ERROR, token);
        }
    }
    else {
        if (expression_type == TYPE_INT) {
            item->data->item_type = TYPE_INT;
        }
        if (expression_type == TYPE_DOUBLE) {
            item->data->item_type = TYPE_DOUBLE;
        }
        if (expression_type == TYPE_STRING) {
            item->data->item_type = TYPE_STRING;
        }
        if (expression_type == TYPE_BOOL) {
            ERROR_HANDLE(TYPE_ERROR, token); //TODO: find out what error code to use
        }
    }
    return true;
}

bool TypeCheck(symtable_item_t *item1, symtable_item_t *item2, int param_index, bool param_handle) {
    data_type_t param_or_func_type;
    if (param_handle) {
        param_or_func_type = item1->data->param_types[param_index];
    }
    else {
        param_or_func_type = item1->data->item_type;
    }
    switch (param_or_func_type) {
        case TYPE_INT:
            if (param_handle) {
                if (item2->data->item_type == TYPE_INT) {
                    if (!item2->data->can_be_nil) {
                        return true;
                    }
                }
            }
            else {
                if (item2->data->item_type == TYPE_INT) {
                    return true;
                }
            }
            break;

        case TYPE_DOUBLE:
                if (param_handle) {
                    if (item2->data->item_type == TYPE_DOUBLE) {
                        if (!item2->data->can_be_nil) {
                            return true;
                        }
                    }
                }
                else {
                    if (item2->data->item_type == TYPE_DOUBLE) {
                        return true;
                    }
                }
            break;

        case TYPE_STRING:
            if (param_handle) {
                if (item2->data->item_type == TYPE_STRING) {
                    if (!item2->data->can_be_nil) {
                        return true;
                    }
                }
            }
            else {
                if (item2->data->item_type == TYPE_STRING) {
                    return true;
                }
            }
            break;

        case TYPE_INT_NIL:
            if (param_handle) {
                if (item2->data->item_type == TYPE_INT) {
                    return true;
                }
            }
            else {
                if (item2->data->item_type == TYPE_INT) {
                    if (item2->data->can_be_nil) {
                        return true;
                    }
                }
            }
            break;

        case TYPE_DOUBLE_NIL:
            if (param_handle) {
                if (item2->data->item_type == TYPE_DOUBLE) {
                    return true;
                }
            }
            else {
                if (item2->data->item_type == TYPE_DOUBLE) {
                    if (item2->data->can_be_nil) {
                        return true;
                    }
                }
            }
            break;

        case TYPE_STRING_NIL:
            if (param_handle) {
                if (item2->data->item_type == TYPE_STRING) {
                    return true;
                }
            }
            else {
                if (item2->data->item_type == TYPE_STRING) {
                    if (item2->data->can_be_nil) {
                        return true;
                    }
                }
            }
            break;

        default:
            return false;
    }
    return false;
}

bool FuncReturnTypeCheck(data_type_t return_expression_type, data_type_t function_type) {
    switch (function_type) {
        case TYPE_INT:
            if (return_expression_type == TYPE_INT) {
                return true;
            }
            break;

        case TYPE_DOUBLE:
            if (return_expression_type == TYPE_DOUBLE) {
                return true;
            }
            break;

        case TYPE_STRING:
            if (return_expression_type == TYPE_STRING) {
                return true;
            }
            break;

        case TYPE_INT_NIL:
            if (return_expression_type == TYPE_INT) {
                return true;
            }
            break;

        case TYPE_DOUBLE_NIL:
            if (return_expression_type == TYPE_DOUBLE) {
                return true;
            }
            break;

        case TYPE_STRING_NIL:
            if (return_expression_type == TYPE_STRING) {
                return true;
            }
            break;

        default:
            return false;
    }
    return false;
}