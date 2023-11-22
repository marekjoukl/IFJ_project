/**
 * @file parser.c
 * @author Marek Joukl (xjoukl00), Ondrej Kožányi (xkozan01)
 * @brief Parser using recursive descent
 */

#include "parser.h"

bool Prog(Lexeme token) {
    // // <PROG> -> EOF
    if (token.kind == EOF) return true;
    // <PROG> -> <SEQUENCE>
    if (token.kind == IDENTIFIER || token.kind == FUNC || token.kind == IF || token.kind == LET || token.kind == WHILE || token.kind == VAR) {
        Sequence(token);
        return true;
    }
    exit(2);
}

bool Sequence(Lexeme token) {
    // <SEQUENCE> -> ε
    if (token.kind == RIGHT_BRACKET || token.kind == EOF) return true;

    // <SEQUENCE> -> <VAR_DEF> IDENTIFIER <VAR_TYPE_DEF> <ASSIGN_VAR> <SEQUENCE>
    if (token.kind == LET || token.kind == VAR) {
        if (!VarDef(token))
            exit(2);
        GETTOKEN();

        if (token.kind != IDENTIFIER)
            exit(2);

        GETTOKEN();

        if (!VarTypeDef(token))
            exit(2);

        GETTOKEN();

        if (!AssignVar(token))
            exit(2);

        GETTOKEN();

        Sequence(token);
        return true;
    }

    
    
    // <SEQUENCE> -> IDENTIFIER <ASSIGN_OR_FUNCTION> <SEQUENCE>
    else if (token.kind == IDENTIFIER) {
        GETTOKEN();

        if (!AssignOrFunction(token))
            exit(2);

        GETTOKEN();

        Sequence(token);
        return true;
    }
    
    //TODO: dokoncit
    // <SEQUENCE> -> IF <IF_EXP> LEFT_BRACKET <SEQUENCE> RIGHT_BRACKET <ELSE_STAT> <SEQUENCE>
    else if (token.kind == IF) {
        GETTOKEN();
        if (!IfExp(token))
            exit(2);

        GETTOKEN();

        if (token.kind != LEFT_BRACKET)
            exit(2);

        GETTOKEN();

        if (!Sequence(token))
            exit(2);

        GETTOKEN();

        if (token.kind != RIGHT_BRACKET)
            exit(2);

        GETTOKEN();

        if (!ElseStat(token))
            exit(2);

        GETTOKEN();

        if (!Sequence(token))
            exit(2);

        return true;
    }
    // <SEQUENCE> -> WHILE <EXPRESSION> LEFT_BRACKET <SEQUENCE> RIGHT_BRACKET <SEQUENCE>
    else if (token.kind == WHILE) {
        GETTOKEN();
        if (!Expression(&token))
            exit(2);
        
        if (token.kind != LEFT_BRACKET)
            exit(2);

        GETTOKEN();
        if(!Sequence(token))
            exit(2);

        GETTOKEN();
        if(token.kind != RIGHT_BRACKET)
            exit(2);

        GETTOKEN();
        if(!Sequence(token))
            exit(2);
            
        return true;
    }
    // <SEQUENCE> -> FUNC IDENTIFIER LEFT_PAR <FIRST_PARAM_DEF> <DEF_FUNCTION> <SEQUENCE>
    else if (token.kind == FUNC) {
        GETTOKEN();
        if (token.kind != IDENTIFIER)
            exit(2);

        GETTOKEN();
        if (token.kind != LEFT_PAR)
            exit(2);

        GETTOKEN();
        if (!FirstParamDef(token))
            exit(2);

        GETTOKEN();
        if (!DefFunction(token))
            exit(2);

        GETTOKEN();
        if (!Sequence(token))
            exit(2);

        return true;
    }

    //ukoncenie rekurzie
    else if (token.kind == RIGHT_BRACKET || token.kind == LEX_EOF) {
        return true;
    }
}

bool AssignOrFunction(Lexeme token) {
    // <ASSIGN_OR_FUNCTION> -> ASSIGNMENT <EXP_OR_CALL>
    if (token.kind == ASSIGNMENT){
        GETTOKEN();
        if (!ExpOrCall(token))
            exit(2);
        return true;
    }
    // <ASSIGN_OR_FUNCTION> -> LEFT_PAR <FIRST_PARAM>
    if (token.kind == LEFT_PAR){
        GETTOKEN();
        if(!FirstParam(token))
            exit(2);
        return true;
    }
    return false;
}

bool DefFunction(Lexeme token) {
    // <DEF_FUNCTION> -> ARROW <TYPE> LEFT_BRACKET <SEQUENCE> <RETURN_FUNCTION> RIGHT_BRACKET
    if (token.kind != ARROW)
        exit(2);
    else {
        GETTOKEN();
        if (!Type(token))
            exit(2);

        GETTOKEN();
        if (token.kind != LEFT_BRACKET)
            exit(2);

        GETTOKEN();
        if (!Sequence(token))
            exit(2);

        GETTOKEN();
        if (!ReturnFunction(token))
            exit(2);

        GETTOKEN();
        if (token.kind != RIGHT_BRACKET)
            exit(2);

        return true;
    }
}

bool ReturnFunction(Lexeme token) {
    // <RETURN_FUNCTION> -> RETURN <EXPRESSION>
    if (token.kind == RETURN){
        GETTOKEN();
        if (!Expression(&token))
            exit(2);

        return true;
    }
    // <RETURN_FUNCTION> -> ε
    if (token.kind == RIGHT_BRACKET)
        return true;
    return false;
}

bool FirstParam(Lexeme token){
    // <FIRST_PARAM> -> RIGHT_PAR
    if (token.kind == RIGHT_PAR)
        return true;
    // <FIRST_PARAM> -> <PARAMS> <PARAMS_N>
    if (token.kind == INTEGER_LIT || token.kind == IDENTIFIER || token.kind == STRING_LIT || token.kind == DOUBLE_LIT) {
        GETTOKEN();
        if (!Params(token))
            exit(2);

        GETTOKEN();
        if(!ParamsN(token))
            exit(2);
        return true;
    }
    return false;
}

bool FirstParamDef(Lexeme token) {
    // <FIRST_PARAM_DEF> -> RIGHT_PAR
    if (token.kind == RIGHT_PAR)
        return true;
    // <FIRST_PARAM_DEF> -> <PARAMS_DEF> <PARAMS_DEF_N>
    if (token.kind == IDENTIFIER || token.kind == UNDERSCORE) {
        GETTOKEN();
        if (!ParamsDef(token))
            exit(2);

        GETTOKEN();
        if(!ParamsDefN(token))
            exit(2);

        return true;
    }
    return false;
}

bool ParamsDef(Lexeme token) {
    // <PARAMS_DEF> -> <PARAMS_NAME_DEF> IDENTIFIER COLON <TYPE>
    if (token.kind == UNDERSCORE || token.kind == IDENTIFIER) {
        GETTOKEN();
        if (!ParamsNameDef(token))
            exit(2);

        GETTOKEN();
        if (token.kind != IDENTIFIER)
            exit(2);

        GETTOKEN();
        if (token.kind != COLON)
            exit(2);

        GETTOKEN();
        if (!Type(token))
            exit(2);

        return true;
    }
    return false;
}

bool ParamsDefN(Lexeme token) {
    // <PARAMS_DEF_N> -> RIGHT_PAR
    if (token.kind == RIGHT_PAR)
        return true;
    // <PARAMS_DEF_N> -> COMMA <PARAMS_DEF> <PARAMS_DEF_N>
    if (token.kind == COMMA){
        GETTOKEN();
        if (!ParamsDef(token))
            exit(2);

        GETTOKEN();
        if(!ParamsDefN(token))
            exit(2);

        return true;
    }
    return false;
}

bool ParamsNameDef(Lexeme token) {
    // <PARAMS_NAME_DEF> -> UNDERSCORE
    if (token.kind == UNDERSCORE)
        return true;
    // <PARAMS_NAME_DEF> -> IDENTIFIER
    if (token.kind == IDENTIFIER)
        return true;
    return false;
}

bool ParamsN(Lexeme token) {
    // <PARAMS_N> -> RIGHT_PAR
    if (token.kind == RIGHT_PAR)
        return true;
    // <PARAMS_N> -> COMMA <PARAMS> <PARAMS_N>
    if (token.kind == COMMA){
        GETTOKEN();
        if (!Params(token))
            exit(2);
        
        GETTOKEN();

        if(!ParamsN(token))
            exit(2);

        return true;
    }
    return false;
}

bool Params(Lexeme token) {
    //  <PARAMS> -> INT_LIT || DOUBLE_LIT || STRING_LIT
    if (token.kind == INTEGER_LIT || token.kind == STRING_LIT || token.kind == DOUBLE_LIT)
        return true;
    // <PARAMS> -> IDENTIFIER <PARAMS_NAME>
    if (token.kind == IDENTIFIER){
        GETTOKEN();
        if(!ParamsName(token))
            exit(2);
        return true;
    }
    return false;
}

bool ParamsName(Lexeme token){
    // <PARAMS_NAME> -> COLON <ID_OR_LIT>
    if (token.kind == COLON){
        GETTOKEN();
        if(!IdOrLit(token))
            exit(2);
        return true;
    }
    // <PARAMS_NAME> -> ε
    if (token.kind == COMMA || token.kind == RIGHT_PAR)
        return true;

    return false;
}

bool IdOrLit(Lexeme token) {
    // <ID_OR_LIT> -> IDENTIFIER || INT_LIT || DOUBLE_LIT || STRING_lIT
    if (token.kind == IDENTIFIER || token.kind == STRING_LIT || token.kind == DOUBLE_LIT || token.kind ==  INTEGER_LIT)
        return true;
    
    return false;
}

bool IfExp(Lexeme token) {
    if (token.kind == LET) {
        GETTOKEN();
        if (token.kind != IDENTIFIER)
            exit(2);
        return true;
    }
    if (token.kind == IDENTIFIER || token.kind == INTEGER_LIT || token.kind == DOUBLE_LIT || token.kind == STRING_LIT || token.kind == LEFT_PAR) {
        GETTOKEN();
        if (!Expression(&token))
            exit(2);
        return true;
    }
    return false;
}

bool ElseStat(Lexeme token) {
    // <ELSE_STAT> -> ELSE LEFT_BRACKET <SEQUENCE> RIGHT_BRACKET
    if (token.kind == ELSE) {
        GETTOKEN();
        if (token.kind != LEFT_BRACKET)
            exit(2);

        GETTOKEN();
        if (!Sequence(token))
            exit(2);

        GETTOKEN();
        if (token.kind != RIGHT_BRACKET)
            exit(2);
        return true;
    }

    return false;
}

bool VarDef(Lexeme token) {
    // <VAR_DEF> -> LET || VAR
    if (token.kind == LET || token.kind == VAR) {
        return true;
    }
    return false;
}

bool VarTypeDef(Lexeme token) {
    // <VAR_TYPE_DEF> -> COLON <TYPE>
    if (token.kind == COLON) {
        GETTOKEN();
        if (!Type(token))
            exit(2);
        return true;
    }
    // <VAR_TYPE_DEF> -> ε
    else if (token.kind == ASSIGNMENT || token.kind == LEX_EOF || token.kind == RIGHT_BRACKET) {
        return true;
    }
    return false;
}

bool Type(Lexeme token) {
    // <TYPE> -> INT <Q_MARK>
    if (token.kind == INT) {
        GETTOKEN();
        if(!QMark(token))
            exit(2);
        return true;
    }
    // <TYPE> -> STRING <Q_MARK>
    else if (token.kind == STRING) {
        GETTOKEN();
        if(!QMark(token))
            exit(2);
        return true;
    }
    // <TYPE> -> DOUBLE <Q_MARK>
    else if (token.kind == DOUBLE) {
        GETTOKEN();
        if(!QMark(token))
            exit(2);
        return true;
    }
    return false;
}

bool QMark(Lexeme token) {
    // <Q_MARK> -> QUESTION_MARK 
    if (token.kind == QUESTION_MARK) {
        return true;
    }
    // <Q_MARK> -> ε
    else if (token.kind == ASSIGNMENT || token.kind == LEX_EOF || token.kind == RIGHT_BRACKET || token.kind == COMMA || token.kind == LEFT_BRACKET || token.kind == RIGHT_PAR) {
        return true;
    }
    return false;
}

bool AssignVar(Lexeme token) {
    // <ASSIGN_VAR> -> ASSIGNMENT <EXP_OR_CALL>
    if (token.kind == ASSIGNMENT) {
        GETTOKEN();
        if (!ExpOrCall(token))
            exit(2);
        return true;
    }
    // <ASSIGN_VAR> -> ε
    else if (token.kind == IDENTIFIER || token.kind == RIGHT_BRACKET || token.kind == LEX_EOF || token.kind == FUNC || token.kind == IF || token.kind == LET || token.kind == WHILE || token.kind == VAR) {
        return true;
    }
    return false;
}

bool ExpOrCall(Lexeme token) {
    if (token.kind == IDENTIFIER) {             //TODO: FIND OUT WHAT RULE TO USE (26. / 27.)
        GETTOKEN();
        if (!CallFunction(token))
            exit(2);
        return true;
    }
    else if ((token.kind == IDENTIFIER /* TODO: look into symtable if id is var or func */) || token.kind == INTEGER_LIT || token.kind == DOUBLE_LIT || token.kind == STRING_LIT || token.kind == LEFT_PAR) {
        GETTOKEN();
        if (!Expression(&token))
            exit(2);
        return true;
    }
    return false;
}

bool CallFunction(Lexeme token) {
    // <CALL_FUNCTION> -> IDENTIFIER LEFT_PAR <FIRST_PARAM>
    if (token.kind != IDENTIFIER)
        exit(2);
    else {
        GETTOKEN();
        if (token.kind != LEFT_PAR)
            exit(2);

        GETTOKEN();
        if (!FirstParam(token))
            exit(2);

        return true;
    }
}

bool Expression(Lexeme *token) {
    // TODO: call precedence analysis
    return true;
}