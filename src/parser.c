/**
 * @file parser.c
 * @author Marek Joukl (xjoukl00)
 * @brief Parser using recursive descent
 */

#include "parser.h"

bool Prog(Lexeme token) {
    if (token.kind == EOF) return true;
    if (token.kind == IDENTIFIER || token.kind == FUNC || token.kind == IF || token.kind == LET || token.kind == WHILE || token.kind == VAR) {
        Sequence(token);
        return true;
    }
    return false;
}

bool Sequence(Lexeme token) {
    if (token.kind == RIGHT_BRACKET || token.kind == EOF) return true;
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

    //TODO: dokoncit
    else if (token.kind == IDENTIFIER) {
        if (!AssignOrFunction(token))
            exit(2);
        return true;
    }
    else if (token.kind == IF) {
        if (!IfExp(token))
            exit(2);
        return true;
    }
    else if (token.kind == WHILE) {
        return true;
    }
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

bool VarDef(Lexeme token) {
    if (token.kind == LET || token.kind == VAR) {
        return true;
    }
    return false;
}

bool VarTypeDef(Lexeme token) {
    if (token.kind == COLON) {
        token = get_next_non_whitespace_lexeme();
        if (!Type(token))
            exit(2);
        return true;
    }

    else if (token.kind == ASSIGNMENT || token.kind == LEX_EOF || token.kind == RIGHT_BRACKET) {
        return true;
    }

    return false;
}

bool Type(Lexeme token) {
    if (token.kind == INT) {
        token = get_next_non_whitespace_lexeme();
        if(!QMark(token))
            exit(2);
        return true;
    }
    else if (token.kind == STRING) {
        token = get_next_non_whitespace_lexeme();
        if(!QMark(token))
            exit(2);
        return true;
    }
    else if (token.kind == DOUBLE) {
        token = get_next_non_whitespace_lexeme();
        if(!QMark(token))
            exit(2);
        return true;
    }
    return false;
}

bool QMark(Lexeme token) {
    if (token.kind == QUESTION_MARK) {
        return true;
    }
    else if (token.kind == ASSIGNMENT || token.kind == LEX_EOF || token.kind == RIGHT_BRACKET || token.kind == COMMA || token.kind == LEFT_BRACKET || token.kind == RIGHT_PAR) {
        return true;
    }
    return false;
}

bool AssignVar(Lexeme token) {
    if (token.kind == ASSIGNMENT) {
        token = get_next_non_whitespace_lexeme();
        if (!ExpOrCall(token))
            exit(2);
        return true;
    }
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
