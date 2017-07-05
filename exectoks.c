#include <stdlib.h>

#include "tokens.h"

token* cur;
short* calcStack;
char sp;

void execRem(void);
void execPrint(void);

void advanceExecutor(void) {
    if (cur->type != TT_NONE) {
        cur = nextToken(cur);
    }
}

void (*executors[])(void) = {
    execRem,
    execPrint,
};

void calcOperation(char op) {
    short top = calcStack[sp--];
    switch (op) {
        case '+':
            calcStack[sp] += top;
            break;
        case '-':
            calcStack[sp] -= top;
            break;
        case '*':
            calcStack[sp] *= top;
            break;
        case '/':
            calcStack[sp] /= top;
            break;
        case '%':
            calcStack[sp] %= top;
            break;
        case '~':
            calcStack[++sp] = -top;
            break;
        case '!':
            calcStack[++sp] = !top;
            break;
        case '<':
            calcStack[sp] = calcStack[sp] < top;
            break;
        case '>':
            calcStack[sp] = calcStack[sp] > top;
            break;
        case '=':
            calcStack[sp] = calcStack[sp] == top;
            break;
        case '{':
            calcStack[sp] = calcStack[sp] <= top;
            break;
        case '}':
            calcStack[sp] = calcStack[sp] >= top;
            break;
        case '#':
            calcStack[sp] = calcStack[sp] != top;
            break;
        case '&':
            calcStack[sp] = calcStack[sp] && top;
            break;
        case '|':
            calcStack[sp] = calcStack[sp] || top;
            break;
    }
}

short calcExpression(void) {
    sp = -1;
    while (1) {
        switch (cur->type) {
            case TT_NONE:
                return calcStack[sp];
            case TT_NUMBER:
                calcStack[++sp] = cur->body.integer;
                break;
            case TT_SYMBOL:
                if (cur->body.symbol == ';') {
                    advanceExecutor();
                    return calcStack[sp];
                }
                calcOperation(cur->body.symbol);
                break;
        }
        advanceExecutor();
    }
}

void execPrint(void) {
    while (1) {
        switch (cur->type) {
            case TT_NONE:
                outputCr();
                return;
            case TT_LITERAL:
                outputNStr(&(cur->body.str));
                break;
            default:
                outputInt(calcExpression());
                break;
        }
        advanceExecutor();
    }
}

void execRem(void) {
}

int executeTokens(token* t) {
    short stackBody[16];
    calcStack = stackBody;
    cur = t;
    if (t->type == TT_COMMAND) {
        advanceExecutor();
        executors[t->body.command]();
    }
}

