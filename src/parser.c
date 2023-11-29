/**
 * @file parser.c
 * @author Marek Joukl (xjoukl00), Ondrej Kožányi (xkozan01)
 * @brief Parser using recursive descent
 */

#include "parser.h"

data_t *CreateData(bool function, int line, bool can_be_nil) {
    data_t *data = malloc(sizeof(data_t));
    if (data == NULL) {
        fprintf(stderr, "Error: parser.c - data_init() - malloc failed\n");
        exit(INTERNAL_ERROR);
    }
    data->is_function = function;
    data->item_type = TYPE_UNDEFINED;
    data->can_be_nil = can_be_nil;
    data->line = line;
    data->value_is_nil = false;
    data->param_count = 0;
    data->param_names = NULL;
    data->params_id = NULL;
    data->param_types = NULL;
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
    Lexeme temp_token;
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
        GETTOKEN();
        if (token->kind != IDENTIFIER)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        item = SymtableSearchAll(stack, token->extra_data.string);
        if (item != NULL) {
            ERROR_HANDLE(DEFINITION_ERROR, token); //TODO: find out what error code to use
        }
        data_t *data = CreateData(true, token->line, token->nil_type);
        SymtableAddItem(stack->array[stack->size - 1], token->extra_data.string, data);
        temp_token = *token;

        GETTOKEN();
        if (token->kind != LEFT_PAR) {
            ERROR_HANDLE(SYNTAX_ERROR, token);
        }

        GETTOKEN();
        if (!FirstParamDef(token, stack, &temp_token)) {
            ERROR_HANDLE(SYNTAX_ERROR, token);
        }

        if (!DefFunction(token, stack, &temp_token)) {
            ERROR_HANDLE(SYNTAX_ERROR, token);
        }

        if (!Prog(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }

    { ERROR_HANDLE(SYNTAX_ERROR, token); }
}

bool Sequence(Lexeme *token, symtable_stack_t *stack) {
    Lexeme temp_token;
    symtable_item_t *item = NULL;

    // <SEQUENCE> -> <VAR_DEF> IDENTIFIER <VAR_TYPE_DEF> <ASSIGN_VAR>
    if (token->kind == LET || token->kind == VAR) {
        if (!VarDef(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (token->kind != IDENTIFIER)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        item = SymtableSearchAll(stack, token->extra_data.string);
        if (item != NULL) {
            ERROR_HANDLE(DEFINITION_ERROR, token);
        }
        temp_token = *token;
        data_t *data = CreateData(false, token->line, token->nil_type);
        SymtableAddItem(stack->array[stack->size - 1], token->extra_data.string, data);

        GETTOKEN();
        if (!VarTypeOrAssign(token, stack, &temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }

    // <SEQUENCE> -> IDENTIFIER <ASSIGN_OR_FUNCTION>
    else if (token->kind == IDENTIFIER) {
        item = SymtableSearchAll(stack, token->extra_data.string);
        if (item == NULL) {
            ERROR_HANDLE(UNDEFINED_VAR_ERROR, token); //TODO: WHAT ERROR CODE TO USE
        }
        temp_token = *token;

        GETTOKEN();
        if (!AssignOrFunction(token, stack, &temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    
    // <SEQUENCE> -> IF <IF_EXP> LEFT_BRACKET <SEQUENCE_N> RIGHT_BRACKET <ELSE_STAT>
    else if (token->kind == IF) {
        GETTOKEN();
        if (!IfExp(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (token->kind != LEFT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN();

        //creating a new frame for if clause
        CREATE_FRAME();

        if (!SequenceN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        //deleting assigned frame
        SymtableStackPop(stack);

        if (token->kind != RIGHT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN();

        if (!ElseStat(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    // <SEQUENCE> -> WHILE <EXPRESSION> LEFT_BRACKET <SEQUENCE_N> RIGHT_BRACKET
    else if (token->kind == WHILE) {
        GETTOKEN();
        if (!Expression(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (token->kind != LEFT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN();

        //creating a new frame for if clause
        CREATE_FRAME();

        if(!SequenceN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        //deleting assigned frame
        SymtableStackPop(stack);

        if(token->kind != RIGHT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN();
            
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

bool AssignOrFunction(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token) {
    // <ASSIGN_OR_FUNCTION> -> ASSIGNMENT <EXP_OR_CALL>
    if (token->kind == ASSIGNMENT){
        GETTOKEN();
        if (!ExpOrCall(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
    // <ASSIGN_OR_FUNCTION> -> LEFT_PAR <FIRST_PARAM>
    if (token->kind == LEFT_PAR){
        GETTOKEN();
        if(!FirstParam(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
    return false;
}

bool DefFunction(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token) {
    // <DEF_FUNCTION> -> <VOID_F> LEFT_BRACKET <SEQUENCE_N> <RETURN_FUNCTION> RIGHT_BRACKET
    if (token->kind == LEFT_BRACKET || token->kind == ARROW) {
        if (!VoidF(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (token->kind != LEFT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN();

        //creating a new frame for function
        Symtable *table = malloc(sizeof(Symtable));
        if (table == NULL) {
            fprintf(stderr, "Error: parser.c - SequenceN() - malloc failed\n");
            exit(INTERNAL_ERROR);
        }
        SymtableInit(table);
        SymtableStackPush(stack, table);

        if (!SequenceN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (!ReturnFunction(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        //deleting assigned frame
        SymtableStackPop(stack);

        if (token->kind != RIGHT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN();

        return true;
    }
    return false;
}

bool VoidF(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token) {
    // <VOID_F> -> ε
    if (token->kind == LEFT_BRACKET) {
        return true;
    }
    // <VOID_F> -> ARROW <TYPE>
    if (token->kind == ARROW) {
        GETTOKEN();
        if (!Type(token, stack, temp_token, false))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
    return false;
}

bool ReturnFunction(Lexeme *token, symtable_stack_t *stack) {
    // <RETURN_FUNCTION> -> RETURN <RETURN_FUNCTION_N>
    if (token->kind == RETURN){
        GETTOKEN();
        if (!ReturnFunctionN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    // <RETURN_FUNCTION> -> ε
    if (token->kind == RIGHT_BRACKET)
        return true;

    return false;
}

bool ReturnFunctionN(Lexeme *token, symtable_stack_t *stack) {
    // <RETURN_FUNCTION_N> -> ε
    if (token->kind == RIGHT_BRACKET)
        return true;
    // <RETURN_FUNCTION_N> -> <EXPRESSION>
    if (token->kind == IDENTIFIER || token->kind == INTEGER_LIT || token->kind == DOUBLE_LIT || token->kind == STRING_LIT || token->kind == LEFT_PAR) {
        if (!Expression(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
    return false;
}

bool FirstParam(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token){
    symtable_item_t *item = SymtableSearchAll(stack, temp_token->extra_data.string);
    // <FIRST_PARAM> -> RIGHT_PAR
    if (token->kind == RIGHT_PAR) {
        if (item->data->param_count != 0) {     //TODO: what about function(_ _) ?
            ERROR_HANDLE(OTHER_SEMANTIC_ERROR, token); //TODO: find out what error code to use
        }
        GETTOKEN();
        return true;
    }
    // <FIRST_PARAM> -> <PARAMS> <PARAMS_N>
    if (token->kind == INTEGER_LIT || token->kind == IDENTIFIER || token->kind == STRING_LIT || token->kind == DOUBLE_LIT) {
        if (!Params(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if(!ParamsN(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
    return false;
}

bool  FirstParamDef(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token) {
    // <FIRST_PARAM_DEF> -> RIGHT_PAR
    if (token->kind == RIGHT_PAR) {
        GETTOKEN();
        return true;
    }
    // <FIRST_PARAM_DEF> -> <PARAMS_DEF> <PARAMS_DEF_N>
    if (token->kind == IDENTIFIER || token->kind == UNDERSCORE) {
        //GETTOKEN();
        if (!ParamsDef(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        //GETTOKEN();
        if(!ParamsDefN(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool ParamsDef(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token) {
    bool first_or_second = true;
    // <PARAMS_DEF> -> <PARAMS_NAME_DEF> <PARAMS_NAME_DEF> COLON <TYPE>
    if (token->kind == UNDERSCORE || token->kind == IDENTIFIER) {
        //GETTOKEN();
        if (!ParamsNameDef(token, stack, temp_token, first_or_second))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        first_or_second = false;

        //GETTOKEN();
        if (token->kind == IDENTIFIER || token->kind == UNDERSCORE) {
            if (!ParamsNameDef(token, stack, temp_token, first_or_second))
                { ERROR_HANDLE(SYNTAX_ERROR, token); }
        }

        if (token->kind != COLON)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        GETTOKEN();
        if (!Type(token, stack, temp_token, true)) //TODO: finish semantics in TYPE
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool ParamsDefN(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token) {
    // <PARAMS_DEF_N> -> RIGHT_PAR
    if (token->kind == RIGHT_PAR) {
        GETTOKEN();
        return true;
    }
    // <PARAMS_DEF_N> -> COMMA <PARAMS_DEF> <PARAMS_DEF_N>
    if (token->kind == COMMA){
        GETTOKEN();
        if (!ParamsDef(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        //GETTOKEN();
        if(!ParamsDefN(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool ParamsNameDef(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token, bool first_or_second) {
    symtable_item_t *item = SymtableSearchAll(stack, temp_token->extra_data.string);
    if (first_or_second) {
        item->data->param_count++;
        item->data->param_names = realloc(item->data->param_names, sizeof(char *) * item->data->param_count);
    }
    else item->data->params_id = realloc(item->data->params_id, sizeof(char*) * item->data->param_count);

    // <PARAMS_NAME_DEF> -> UNDERSCORE
    if (token->kind == UNDERSCORE) {
        if (first_or_second) item->data->param_names[item->data->param_count - 1] = NULL;
        else item->data->params_id[item->data->param_count - 1] = NULL;
        GETTOKEN();
        return true;
    }
    // <PARAMS_NAME_DEF> -> IDENTIFIER
    if (token->kind == IDENTIFIER) {
        if (first_or_second) item->data->param_names[item->data->param_count - 1] = token->extra_data.string;
        else item->data->params_id[item->data->param_count - 1] = token->extra_data.string;
        GETTOKEN();
        return true;
    }
    return false;
}

bool ParamsN(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token) {
    // <PARAMS_N> -> RIGHT_PAR
    if (token->kind == RIGHT_PAR) {
        GETTOKEN();
        return true;
    }

    // <PARAMS_N> -> COMMA <PARAMS> <PARAMS_N>
    if (token->kind == COMMA){
        GETTOKEN();
        if (!Params(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if(!ParamsN(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool Params(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token) {
    Lexeme param_id;
    symtable_item_t *item = SymtableSearchAll(stack, temp_token->extra_data.string);
    //  <PARAMS> -> INT_LIT
    if (token->kind == INTEGER_LIT) {
        if (item->data->param_types[item->data->param_count_current] != TYPE_INT) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        item->data->param_count_current++;
        GETTOKEN();
        return true;
    }
    //  <PARAMS> -> STRING_LIT
    if (token->kind == STRING_LIT) {
        if (item->data->param_types[item->data->param_count_current] != TYPE_STRING) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        item->data->param_count_current++;
        GETTOKEN();
        return true;
    }
    //  <PARAMS> -> DOUBLE_LIT
    if (token->kind == DOUBLE_LIT) {
        if (item->data->param_types[item->data->param_count_current] != TYPE_DOUBLE) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        item->data->param_count_current++;
        GETTOKEN();
        return true;
    }
    // <PARAMS> -> IDENTIFIER <PARAMS_NAME>
    if (token->kind == IDENTIFIER){
        param_id = *token;
        GETTOKEN();
        if(!ParamsName(token, stack, temp_token, &param_id))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        item->data->param_count_current++;
        return true;
    }
    // <PARAMS> -> NIL
    if (token->kind == NIL) { //TODO: finish semantics in NIL case
        GETTOKEN();
        return true;
    }

    return false;
}

bool ParamsName(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token, Lexeme *param_name_or_id) {
    symtable_item_t *function = SymtableSearchAll(stack, temp_token->extra_data.string);
    // <PARAMS_NAME> -> COLON <ID_OR_LIT>
    if (token->kind == COLON){
        if (strcmp(param_name_or_id->extra_data.string, function->data->param_names[function->data->param_count_current]) != 0) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        GETTOKEN();
        if(!IdOrLit(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    // <PARAMS_NAME> -> ε
    if (token->kind == COMMA || token->kind == RIGHT_PAR) {
        symtable_item_t *param_item = SymtableSearchAll(stack, param_name_or_id->extra_data.string);
        if (param_item == NULL) {
            ERROR_HANDLE(UNDEFINED_VAR_ERROR, token);
        }
        if (function->data->param_types[function->data->param_count_current] != param_item->data->item_type) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token); //TODO: find out what error code to use
        }
        return true;
    }

    return false;
}

bool IdOrLit(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token) {
    symtable_item_t *function = SymtableSearchAll(stack, temp_token->extra_data.string);
    // <ID_OR_LIT> -> IDENTIFIER || INT_LIT || DOUBLE_LIT || STRING_lIT || NIL
    if (token->kind == IDENTIFIER) {
        symtable_item_t *param = SymtableSearchAll(stack, token->extra_data.string);
        if (param == NULL) {
            ERROR_HANDLE(UNDEFINED_VAR_ERROR, token);
        }
        if (function->data->param_types[function->data->param_count_current] != param->data->item_type) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        GETTOKEN();
        return true;
    }
    if (token->kind == STRING_LIT) {
        if (function->data->param_types[function->data->param_count_current] != TYPE_STRING) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        GETTOKEN();
        return true;
    }
    if (token->kind == DOUBLE_LIT) {
        if (function->data->param_types[function->data->param_count_current] != TYPE_DOUBLE) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        GETTOKEN();
        return true;
    }
    if (token->kind ==  INTEGER_LIT) {
        if (function->data->param_types[function->data->param_count_current] != TYPE_INT) {
            ERROR_HANDLE(PARAMETER_TYPE_ERROR, token);
        }
        GETTOKEN();
        return true;
    }
    if (token->kind == NIL) { //TODO: what to do with nil lit parameter
        GETTOKEN();
        return true;
    }
    return false;
}

bool IfExp(Lexeme *token, symtable_stack_t *stack) {
    // <IF_EXP> -> LET IDENTIFIER
    if (token->kind == LET) {
        GETTOKEN();
        if (token->kind == IDENTIFIER) {
            GETTOKEN();
            return true;
        }
        return false;
    }
    // <IF_EXP> -> <EXPRESSION>
    if (token->kind == IDENTIFIER || token->kind == INTEGER_LIT || token->kind == DOUBLE_LIT || token->kind == STRING_LIT || token->kind == LEFT_PAR) {
        if (!Expression(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool ElseStat(Lexeme *token, symtable_stack_t *stack) {
    // <ELSE_STAT> -> ELSE LEFT_BRACKET <SEQUENCE_N> RIGHT_BRACKET
    if (token->kind == ELSE) {
        GETTOKEN();
        if (token->kind != LEFT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN();

        //creating a new frame for else clause
        CREATE_FRAME();

        if (!SequenceN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        //deleting assigned frame
        SymtableStackPop(stack);

        if (token->kind != RIGHT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN();

        return true;
    }

    return false;
}

bool VarDef(Lexeme *token, symtable_stack_t *stack) {
    // <VAR_DEF> -> LET
    if (token->kind == LET) {
        GETTOKEN();
        return true;
    }

    // <VAR_DEF> -> VAR
    if (token->kind == VAR) {
        GETTOKEN();
        return true;
    }

    return false;
}

bool VarTypeOrAssign(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token) {
    // <VAR_TYPE_OR_ASSIGN> -> COLON <TYPE> <ASSIGN_VAR>
    if (token->kind == COLON) {
        GETTOKEN();
        if (!Type(token, stack, temp_token, false))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (!AssignVar(token, stack, temp_token))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    // <VAR_TYPE_OR_ASSIGN> -> ASSIGNMENT <EXP_OR_CALL>
    else if (token->kind == ASSIGNMENT) {
        GETTOKEN();
        if (!ExpOrCall(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool Type(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token, bool param) {
    symtable_item_t *item = SymtableSearchAll(stack, temp_token->extra_data.string);
    item->data->param_types = realloc(item->data->param_types, sizeof(int) * item->data->param_count);
    // <TYPE> -> INT
    if (token->kind == INT) {
        GETTOKEN();
        if (param) item->data->param_types[item->data->param_count - 1] = TYPE_INT;
        else item->data->item_type = TYPE_INT;
        return true;
    }
    // <TYPE> -> STRING
    else if (token->kind == STRING) {
        GETTOKEN();
        if (param) item->data->param_types[item->data->param_count - 1] = TYPE_STRING;
        else item->data->item_type = TYPE_STRING;
        return true;
    }
    // <TYPE> -> DOUBLE
    else if (token->kind == DOUBLE) {
        GETTOKEN();
        if (param) item->data->param_types[item->data->param_count - 1] = TYPE_DOUBLE;
        else item->data->item_type = TYPE_DOUBLE;
        return true;
    }
    return false;
}

bool AssignVar(Lexeme *token, symtable_stack_t *stack, Lexeme *temp_token) {
    // <ASSIGN_VAR> -> ASSIGNMENT <EXP_OR_CALL>
    if (token->kind == ASSIGNMENT) {
        GETTOKEN();
        if (!ExpOrCall(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
    // <ASSIGN_VAR> -> ε
    else if (token->kind == IDENTIFIER || token->kind == LEX_EOF || token->kind == FUNC || token->kind == IF || token->kind == LET || token->kind == WHILE || token->kind == VAR || token->kind == RIGHT_BRACKET || token->kind == RETURN) {
        return true;
    }
    return false;
}

bool ExpOrCall(Lexeme *token, symtable_stack_t *stack) {
    symtable_item_t *item = NULL;
    if (token->kind == IDENTIFIER) {
        item = SymtableSearchAll(stack, token->extra_data.string);
        if (item == NULL) {
            GETTOKEN();
            if (token->kind == LEFT_PAR)
                { ERROR_HANDLE(DEFINITION_ERROR, token); }
            else
                { ERROR_HANDLE(UNDEFINED_VAR_ERROR, token); }
        }
    }

    if (token->kind == IDENTIFIER && item->data->is_function) {
        if (!CallFunction(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }

    else if (token->kind == IDENTIFIER || token->kind == INTEGER_LIT || token->kind == DOUBLE_LIT || token->kind == STRING_LIT || token->kind == LEFT_PAR) {
        if (!Expression(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
    return false;
}

bool CallFunction(Lexeme *token, symtable_stack_t *stack) {
    // <CALL_FUNCTION> -> IDENTIFIER LEFT_PAR <FIRST_PARAM>
    if (token->kind == IDENTIFIER) {
        GETTOKEN();
        if (token->kind != LEFT_PAR)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN();

        if (!FirstParam(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool Expression(Lexeme *token, symtable_stack_t *stack) {
    //TODO: pridat stack do precedencnej
    return precedent_analysys(token);
}
