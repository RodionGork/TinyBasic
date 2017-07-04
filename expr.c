#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "tokenint.h"
#include "expr.h"
#include "editor.h"

int parseExprUnary() {
    if (!charInStr(*getCurTokPos(), "-!")) {
        return 0;
    }
    parseSymbol();
    if (prevTok->body.symbol = '-') {
        prevTok->body.symbol = '~';
    }
    return 1;
}

char parseExprVal(void) {
    if (parseNumber()) {
        if (*getCurTokPos() == '(') {
            setTokenError(getCurTokPos(), 9);
            return 'e';
        }
        return '+';
    }
    if (parseName(0)) {
        if (*getCurTokPos() == '(') {
            prevTok->type = TT_FUNCTION;
            skipTokenInInput(1);
            return 'f';
        }
        prevTok->type = TT_VARIABLE;
        return '+';
    }
    if (*getCurTokPos() == '(') {
        parseSymbol();
        return '(';
    }
    if (parseExprUnary()) {
        return '1';
    }
    setTokenError(getCurTokPos(), 9);
    return 'e';
}

char parseExprBop(void) {
    char c = *getCurTokPos();
    if (c == 0 || c == ';') {
        return 's';
    }
    if (*getCurTokPos() == ')') {
        return ')';
    }
    if (*getCurTokPos() == ',') {
        return ',';
    }
    if (charInStr(*getCurTokPos(), "+-*^/%<>=&|")) {
        parseSymbol();
        return '1';
    }
    setTokenError(getCurTokPos(), 9);
    return 'e';
}

char parseExprRbr(int brCount, int argCount) {
    if (brCount < 1) {
        setTokenError(getCurTokPos(), 9);
        return 'e';
    } else {
        parseSymbol();
        if (argCount > 0) {
            prevTok->type = TT_FUNC_END;
            prevTok->body.symbol = argCount;
        }
        return '+';
    }
}

char parseExprComma(char inFunc) {
    if (inFunc == 0) {
        setTokenError(getCurTokPos(), 9);
        return 'e';
    } else {
        parseSymbol();
        return '1';
    }
}

token* convertToRpn(token* next) {
    char buf[MAX_LINE_LEN * 2];
    char opstack[16];
    char sp = -1;
    curTok->type = TT_ERROR;
    memcpy(buf, next, ((void*) curTok) - ((void*) next) + 1);
    curTok = next;
    next = (void*) buf;
    return start;
}

int parseExpression(void) {
    token* startTok = curTok;
    char funcBrackets[16];
    char iFuncBr = 0;
    char state = '1';
    funcBrackets[iFuncBr] = 0;
    while (state != 's') {
        switch (state) {
            case '1':
                state = parseExprVal();
                break;
            case 'f':
                funcBrackets[++iFuncBr] = 1;
                state = '1';
                break;
            case '(':
                funcBrackets[++iFuncBr] = 0;
                state = '1';
                break;
            case '+':
                state = parseExprBop();
                break;
            case ')':
                state = parseExprRbr(iFuncBr, funcBrackets[iFuncBr]);
                iFuncBr -= 1;
                break;
            case ',':
                state = parseExprComma(funcBrackets[iFuncBr]++);
                break;
            case 'e':
                return 0;
        }
    }
    convertToRpn(startTok);
    return 1;
}


