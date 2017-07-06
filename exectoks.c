#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "utils.h"

#define MAX_VARS 40

typedef struct varHolder {
    short name;
    short value;
} __attribute__((packed)) varHolder;

token* cur;
short* calcStack;
char sp;

void execRem(void);
void execPrint(void);
void execInput(void);

void (*executors[])(void) = {
    execRem,
    execPrint,
    execInput,
};

varHolder vars[MAX_VARS];

char numVars = 0;

short shortVarName(nstring* name) {
    short n = name->text[0];
    if (name->len > 1) {
        n += name->text[1] * 127;
    }
    return n;
}

char findVar(short name) {
    short hi = numVars;
    short lo = 0;
    short mid;
    while (hi > lo) {
        mid = (hi + lo) / 2;
        if (vars[mid].name < name) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return lo;
}

short getVar(short name) {
    char i = findVar(name);
    return (vars[i].name == name) ? vars[i].value : 0;
}

void setVar(short name, short value) {
    char i = findVar(name);
    if (vars[i].name != name) {
        if (i < numVars) {
            memmove(vars + i + 1, vars + i, sizeof(varHolder) * (numVars - i));
        }
        vars[i].name = name;
        numVars += 1;
    }
    vars[i].value = value;
}

void advanceExecutor(void) {
    if (cur->type != TT_NONE) {
        cur = nextToken(cur);
    }
}

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
            case TT_SEPARATOR:
                return calcStack[sp];
            case TT_NUMBER:
                calcStack[++sp] = cur->body.integer;
                break;
            case TT_VARIABLE:
                calcStack[++sp] = getVar(shortVarName(&(cur->body.str)));
                break;
            case TT_SYMBOL:
                calcOperation(cur->body.symbol);
                break;
        }
        advanceExecutor();
    }
}

void execAssignment(void) {
    short varname = shortVarName(&(cur->body.str));
    advanceExecutor();
    advanceExecutor();
    setVar(varname, calcExpression());
}

void execInput(void) {
    char s[16];
    while (1) {
        switch (cur->type) {
            case TT_NONE:
                return;
            case TT_SEPARATOR:
                break;
            case TT_VARIABLE:
                outputStr("? ");
                input(s, sizeof(s));
                setVar(shortVarName(&(cur->body.str)), atoi(s));
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
            case TT_SEPARATOR:
                break;
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
    } else {
        execAssignment();
    }
}

