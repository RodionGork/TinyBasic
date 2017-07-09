#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "editor.h"
#include "utils.h"

typedef struct varHolder {
    short name;
    short value;
} __attribute__((packed)) varHolder;

token* tcur;
short* calcStack;
short nextLineNum = 1;
short sp;

void execRem(void);
void execPrint(void);
void execInput(void);
void execIf(void);
void execGoto(void);
void execGosub(void);
void execReturn(void);
void execEnd(void);
void execPin(void);
void execDelay(void);

void (*executors[])(void) = {
    execRem,
    execPrint,
    execInput,
    execIf,
    execGoto,
    execGosub,
    execReturn,
    execEnd,
    execPin,
    execDelay,
};

varHolder* vars;

char numVars = 0;

void initTokenExecutor(char* space, short size) {
    numVars = 0;
    sp = (size / sizeof(*calcStack));
    vars = (varHolder*)(void*)space;
    calcStack = (short*)(void*)space;
}

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
    if (tcur->type != TT_NONE) {
        tcur = nextToken(tcur);
    }
}

void calcOperation(char op) {
    short top = calcStack[sp++];
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
            calcStack[--sp] = -top;
            break;
        case '!':
            calcStack[--sp] = !top;
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

void calcFunction(nstring* name) {
    if (memcmp(&(name->text), "PIN", 3) == 0) {
        calcStack[sp] = pinread(calcStack[sp]);
    } else if (memcmp(&(name->text), "ADC", 3) == 0) {
        calcStack[sp] = adcread(calcStack[sp]);
    } else if (memcmp(&(name->text), "ABS", 3) == 0) {
        calcStack[sp] = abs(calcStack[sp]);
    } else {
        calcStack[sp] = 0;
    }
}

short calcExpression(void) {
    while (1) {
        switch (tcur->type) {
            case TT_NONE:
            case TT_SEPARATOR:
                return calcStack[sp++];
            case TT_NUMBER:
                calcStack[--sp] = tcur->body.integer;
                break;
            case TT_VARIABLE:
                calcStack[--sp] = getVar(shortVarName(&(tcur->body.str)));
                break;
            case TT_SYMBOL:
                calcOperation(tcur->body.symbol);
                break;
            case TT_FUNCTION:
                calcFunction(&(tcur->body.str));
                break;
        }
        advanceExecutor();
    }
}

void execAssignment(void) {
    short varname = shortVarName(&(tcur->body.str));
    advanceExecutor();
    advanceExecutor();
    setVar(varname, calcExpression());
}

void execRem(void) {
    while (tcur->type != TT_NONE) {
        advanceExecutor();
    }
}

void execPrint(void) {
    while (1) {
        switch (tcur->type) {
            case TT_NONE:
                outputCr();
                return;
            case TT_SEPARATOR:
                break;
            case TT_LITERAL:
                outputNStr(&(tcur->body.str));
                break;
            default:
                outputInt(calcExpression());
                break;
        }
        advanceExecutor();
    }
}

void execInput(void) {
    char s[16];
    while (1) {
        switch (tcur->type) {
            case TT_NONE:
                return;
            case TT_SEPARATOR:
                break;
            case TT_VARIABLE:
                outputStr("? ");
                input(s, sizeof(s));
                setVar(shortVarName(&(tcur->body.str)), atoi(s));
                break;
        }
        advanceExecutor();
    }
}

void execIf(void) {
    if (calcExpression() == 0) {
        while (tcur->type != TT_NONE) {
            advanceExecutor();
        }
    } else {
        advanceExecutor();
    }
}

void execGoto(void) {
    nextLineNum = tcur->body.integer;
    advanceExecutor();
}

void execGosub(void) {
    calcStack[--sp] = nextLineNum;
    nextLineNum = tcur->body.integer;
    advanceExecutor();
}

void execReturn(void) {
    nextLineNum = calcStack[sp++];
}

void execEnd(void) {
    nextLineNum = 32767;
}

void execPin(void) {
    char pin = calcExpression();
    advanceExecutor();
    pinout(pin, calcExpression());
}

void execDelay(void) {
    sysdelay(calcExpression());
}

char executeTokens(token* t) {
    tcur = t;
    while (t->type != TT_NONE) {
        if (t->type == TT_COMMAND) {
            advanceExecutor();
            executors[t->body.command]();
        } else {
            execAssignment();
        }
        t = tcur;
    }
    return 1;
}

char executeStep(char* lineBuf, token* tokenBuf) {
    prgline* p = findLine(nextLineNum);
    if (p->num == 0) {
        outputStr("End of code\n");
        return 1;
    }
    nextLineNum = p->num + 1;
    memcpy(lineBuf, p->str.text, p->str.len);
    lineBuf[p->str.len] = 0;
    parseLine(lineBuf, tokenBuf);
    executeTokens(tokenBuf);
    return 0;
}

