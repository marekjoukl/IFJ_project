/**
 * @file parser.c
 * @author Marek Joukl (xjoukl00)
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
    return false;
}

bool Sequence(Lexeme token) {
    // <SEQUENCE> -> ε
    if (token.kind == RIGHT_BRACKET || token.kind == EOF) return true;

    // <SEQUENCE> -> <VAR_DEF> IDENTIFIER <VAR_TYPE_DEF> <ASSIGN_VAR> <SEQUENCE>
    if (token.kind == LET || token.kind == VAR) {
        if (!VarDef(token))
            exit(2);
        token = get_next_non_whitespace_lexeme();

        if (token.kind != IDENTIFIER)
            exit(2);

        token = get_next_non_whitespace_lexeme();

        if (!VarTypeDef(token))
            exit(2);

        token = get_next_non_whitespace_lexeme();

        if (!AssignVar(token))
            exit(2);

        token = get_next_non_whitespace_lexeme();

        Sequence(token);
        return true;
    }

    
    // <SEQUENCE> -> IDENTIFIER <ASSIGN_OR_FUNCTION> <SEQUENCE>
    else if (token.kind == IDENTIFIER) {
        token = get_next_non_whitespace_lexeme();

        if (!AssignOrFunction(token))
            exit(2);

        token = get_next_non_whitespace_lexeme();

        Sequence(token);
        return true;
    }
    
    //TODO: dokoncit
    // <SEQUENCE> -> IF <IF_EXP> LEFT_BRACKET <SEQUENCE> RIGHT_BRACKET <ELSE_STAT> <SEQUENCE>
    else if (token.kind == IF) {
        if (!IfExp(token))
            exit(2);
        return true;
    }
    // <SEQUENCE> -> WHILE <EXPRESSION> LEFT_BRACKET <SEQUENCE> RIGHT_BRACKET <SEQUENCE>
    else if (token.kind == WHILE) {
        return true;
    }
    // <SEQUENCE> -> FUNC IDENTIFIER LEFT_PAR <FIRST_PARAM_DEF> <DEF_FUNCTION> <SEQUENCE>
    else if (token.kind == FUNC) {
        if (!DefFunction(token))
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
        token = get_next_non_whitespace_lexeme();
        if (!ExpOrCall(token))
            exit(2);
        return true;
    }
    // <ASSIGN_OR_FUNCTION> -> LEFT_PAR <FIRST_PARAM>
    if (token.kind == LEFT_PAR){
        token = get_next_non_whitespace_lexeme();
        if(!FirstParam(token))
            exit(2);
        return true;
    }
    return false;
}

bool FirstParam(Lexeme token){
    // <FIRST_PARAM> -> RIGHT_PAR
    if (token.kind == RIGHT_PAR)
        return true;
    // <FIRST_PARAM> -> <PARAMS> <PARAMS_N>
    if (token.kind == INTEGER_LIT || token.kind == IDENTIFIER || token.kind == STRING_LIT || token.kind == DOUBLE_LIT) {
        token = get_next_non_whitespace_lexeme();
        if (!Params(token))
            exit(2);

        token = get_next_non_whitespace_lexeme();

        if(!ParamsN(token))
            exit(2);
        return true;
    }
    return false;
}

bool ParamsN(Lexeme token) {
    // <PARAMS_N> -> RIGHT_PAR
    if (token.kind == RIGHT_PAR)
        return true;
    // <PARAMS_N> -> COMMA <PARAMS> <PARAMS_N>
    if (token.kind == COMMA){
        token = get_next_non_whitespace_lexeme();
        if (!Params(token))
            exit(2);
        
        token = get_next_non_whitespace_lexeme();

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
        token = get_next_non_whitespace_lexeme();
        if(!ParamsName(token))
            exit(2);
        return true;
    }
    return false;
}

bool ParamsName(Lexeme token){
    // <PARAMS_NAME> -> COLON <ID_OR_LIT>
    if (token.kind == COLON){
        token = get_next_non_whitespace_lexeme();
        if(!IdOrLit(token))
            exit(2);
        return true;
    }
    // <PARAMS_NAME> -> ε
    if (token.kind == COMMA || RIGHT_PAR)
        return true;

    return false;
}

bool IdOrLit(Lexeme token) {
    // <ID_OR_LIT> -> IDENTIFIER || INT_LIT || DOUBLE_LIT || STRING_lIT
    if (token.kind == IDENTIFIER || token.kind == STRING_LIT || token.kind == DOUBLE_LIT || token.kind ==  INTEGER_LIT)
        return true;
    
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
        token = get_next_non_whitespace_lexeme();
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
        token = get_next_non_whitespace_lexeme();
        if(!QMark(token))
            exit(2);
        return true;
    }
    // <TYPE> -> STRING <Q_MARK>
    else if (token.kind == STRING) {
        token = get_next_non_whitespace_lexeme();
        if(!QMark(token))
            exit(2);
        return true;
    }
    // <TYPE> -> DOUBLE <Q_MARK>
    else if (token.kind == DOUBLE) {
        token = get_next_non_whitespace_lexeme();
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
        token = get_next_non_whitespace_lexeme();
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
        token = get_next_non_whitespace_lexeme();
        if (!CallFunction(token))
            exit(2);
        return true;
    }
    else if ((token.kind == IDENTIFIER /* TODO: look into symtable if id is var or func */) || token.kind == INTEGER_LIT || token.kind == DOUBLE_LIT || token.kind == STRING_LIT || token.kind == LEFT_PAR) {
        token = get_next_non_whitespace_lexeme();
        if (!Expression(token))
            exit(2);
        return true;
    }
    return false;
}
