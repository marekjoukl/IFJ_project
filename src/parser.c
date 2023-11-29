/**
 * @file parser.c
 * @author Marek Joukl (xjoukl00), Ondrej Kožányi (xkozan01)
 * @brief Parser using recursive descent
 */

#include "parser.h"

void StartParser() {
    symtable_stack_t *stack = SymtableStackInit();
    Symtable table;
    Lexeme *token = NULL;
    GETTOKEN();
    Prog(token, stack);
}

bool Prog(Lexeme *token, symtable_stack_t *stack) {
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

        GETTOKEN();
        if (token->kind != LEFT_PAR) {
            ERROR_HANDLE(SYNTAX_ERROR, token);
        }

        GETTOKEN();
        if (!FirstParamDef(token, stack)) {
            ERROR_HANDLE(SYNTAX_ERROR, token);
        }

        if (!DefFunction(token, stack)) {
            ERROR_HANDLE(SYNTAX_ERROR, token);
        }
            

        if (!Prog(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }

    { ERROR_HANDLE(SYNTAX_ERROR, token); }
}

bool Sequence(Lexeme *token, symtable_stack_t *stack) {
    // <SEQUENCE> -> <VAR_DEF> IDENTIFIER <VAR_TYPE_DEF> <ASSIGN_VAR>
    if (token->kind == LET || token->kind == VAR) {
        if (!VarDef(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (token->kind != IDENTIFIER)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN();

        if (!VarTypeOrAssign(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }

    // <SEQUENCE> -> IDENTIFIER <ASSIGN_OR_FUNCTION>
    else if (token->kind == IDENTIFIER) {
        GETTOKEN();
        if (!AssignOrFunction(token, stack))
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

        if (!SequenceN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

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

        if(!SequenceN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

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

bool AssignOrFunction(Lexeme *token, symtable_stack_t *stack) {
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
        if(!FirstParam(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
    return false;
}

bool DefFunction(Lexeme *token, symtable_stack_t *stack) {
    // <DEF_FUNCTION> -> <VOID_F> LEFT_BRACKET <SEQUENCE_N> <RETURN_FUNCTION> RIGHT_BRACKET
    if (token->kind == LEFT_BRACKET || token->kind == ARROW) {
        if (!VoidF(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (token->kind != LEFT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN();

        if (!SequenceN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (!ReturnFunction(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (token->kind != RIGHT_BRACKET)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        GETTOKEN();

        return true;
    }
    return false;
}

bool VoidF(Lexeme *token, symtable_stack_t *stack) {
    // <VOID_F> -> ε
    if (token->kind == LEFT_BRACKET) {
        return true;
    }
    // <VOID_F> -> ARROW <TYPE>
    if (token->kind == ARROW) {
        GETTOKEN();
        if (!Type(token, stack))
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

bool FirstParam(Lexeme *token, symtable_stack_t *stack){
    // <FIRST_PARAM> -> RIGHT_PAR
    if (token->kind == RIGHT_PAR) {
        GETTOKEN();
        return true;
    }
    // <FIRST_PARAM> -> <PARAMS> <PARAMS_N>
    if (token->kind == INTEGER_LIT || token->kind == IDENTIFIER || token->kind == STRING_LIT || token->kind == DOUBLE_LIT) {
        if (!Params(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if(!ParamsN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
    return false;
}

bool  FirstParamDef(Lexeme *token, symtable_stack_t *stack) {
    // <FIRST_PARAM_DEF> -> RIGHT_PAR
    if (token->kind == RIGHT_PAR) {
        GETTOKEN();
        return true;
    }
    // <FIRST_PARAM_DEF> -> <PARAMS_DEF> <PARAMS_DEF_N>
    if (token->kind == IDENTIFIER || token->kind == UNDERSCORE) {
        //GETTOKEN();
        if (!ParamsDef(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        //GETTOKEN();
        if(!ParamsDefN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool ParamsDef(Lexeme *token, symtable_stack_t *stack) {
    // <PARAMS_DEF> -> <PARAMS_NAME_DEF> IDENTIFIER COLON <TYPE>
    if (token->kind == UNDERSCORE || token->kind == IDENTIFIER) {
        //GETTOKEN();
        if (!ParamsNameDef(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        //GETTOKEN();
        if (token->kind != IDENTIFIER)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        GETTOKEN();
        if (token->kind != COLON)
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        GETTOKEN();
        if (!Type(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool ParamsDefN(Lexeme *token, symtable_stack_t *stack) {
    // <PARAMS_DEF_N> -> RIGHT_PAR
    if (token->kind == RIGHT_PAR) {
        GETTOKEN();
        return true;
    }
    // <PARAMS_DEF_N> -> COMMA <PARAMS_DEF> <PARAMS_DEF_N>
    if (token->kind == COMMA){
        GETTOKEN();
        if (!ParamsDef(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        //GETTOKEN();
        if(!ParamsDefN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool ParamsNameDef(Lexeme *token, symtable_stack_t *stack) {
    // <PARAMS_NAME_DEF> -> UNDERSCORE
    if (token->kind == UNDERSCORE) {
        GETTOKEN();
        return true;
    }
    // <PARAMS_NAME_DEF> -> IDENTIFIER
    if (token->kind == IDENTIFIER) {
        GETTOKEN();
        return true;
    }
    return false;
}

bool ParamsN(Lexeme *token, symtable_stack_t *stack) {
    // <PARAMS_N> -> RIGHT_PAR
    if (token->kind == RIGHT_PAR) {
        GETTOKEN();
        return true;
    }

    // <PARAMS_N> -> COMMA <PARAMS> <PARAMS_N>
    if (token->kind == COMMA){
        GETTOKEN();
        if (!Params(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if(!ParamsN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    return false;
}

bool Params(Lexeme *token, symtable_stack_t *stack) {
    //  <PARAMS> -> INT_LIT || DOUBLE_LIT || STRING_LIT
    if (token->kind == INTEGER_LIT || token->kind == STRING_LIT || token->kind == DOUBLE_LIT) {
        GETTOKEN();
        return true;
    }
    // <PARAMS> -> IDENTIFIER <PARAMS_NAME>
    if (token->kind == IDENTIFIER){
        GETTOKEN();
        if(!ParamsName(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }
        return true;
    }
    // <PARAMS> -> NIL
    if (token->kind == NIL) {
        GETTOKEN();
        return true;
    }

    return false;
}

bool ParamsName(Lexeme *token, symtable_stack_t *stack){
    // <PARAMS_NAME> -> COLON <ID_OR_LIT>
    if (token->kind == COLON){
        GETTOKEN();
        if(!IdOrLit(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        return true;
    }
    // <PARAMS_NAME> -> ε
    if (token->kind == COMMA || token->kind == RIGHT_PAR)
        return true;

    return false;
}

bool IdOrLit(Lexeme *token, symtable_stack_t *stack) {
    // <ID_OR_LIT> -> IDENTIFIER || INT_LIT || DOUBLE_LIT || STRING_lIT || NIL
    if (token->kind == IDENTIFIER || token->kind == STRING_LIT || token->kind == DOUBLE_LIT || token->kind ==  INTEGER_LIT || token->kind == NIL) {
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

        if (!SequenceN(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

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

bool VarTypeOrAssign(Lexeme *token, symtable_stack_t *stack) {
    // <VAR_TYPE_OR_ASSIGN> -> COLON <TYPE> <ASSIGN_VAR>
    if (token->kind == COLON) {
        GETTOKEN();
        if (!Type(token, stack))
            { ERROR_HANDLE(SYNTAX_ERROR, token); }

        if (!AssignVar(token, stack))
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

bool Type(Lexeme *token, symtable_stack_t *stack) {
    // <TYPE> -> INT
    if (token->kind == INT) {
        GETTOKEN();
        return true;
    }
    // <TYPE> -> STRING
    else if (token->kind == STRING) {
        GETTOKEN();
        return true;
    }
    // <TYPE> -> DOUBLE
    else if (token->kind == DOUBLE) {
        GETTOKEN();
        return true;
    }
    return false;
}

bool AssignVar(Lexeme *token, symtable_stack_t *stack) {
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

    if (token->kind == IDENTIFIER && item->data.is_function) {
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
