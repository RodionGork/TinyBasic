#include <stdlib.h>
#include <string.h>

#include "mytypes.h"
#include "tokens.h"
#include "editor.h"
#include "utils.h"
#include "extern.h"

static token* curTok;
numeric* calcStack;
short nextLineNum = 1;
short sp, spInit;
varHolder* vars;
char numVars;
labelCacheElem* labelCache;
short labelsCached;

void execRem(void);
void execPrint(void);
void execInput(void);
void execIf(void);
void execGoto(void);
void execGosub(void);
void execReturn(void);
void execEnd(void);

void (*executors[])(void) = {
    execRem,
    execPrint,
    execInput,
    execIf,
    execGoto,
    execGosub,
    execReturn,
    execEnd,
};

void resetTokenExecutor(void) {
    numVars = 0;
    sp = spInit;
}

void initTokenExecutor(char* space, short size) {
    spInit = (size / sizeof(*calcStack));
    vars = (varHolder*)(void*)space;
    calcStack = (numeric*)(void*)space;
    resetTokenExecutor();
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

numeric getVar(short name) {
    char i = findVar(name);
    return (vars[i].name == name) ? vars[i].value : 0;
}

void setVar(short name, numeric value) {
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

short findLabel(short num) {
    short hi = labelsCached;
    short lo = 0;
    short mid;
    while (hi > lo) {
        mid = (hi + lo) / 2;
        if (labelCache[mid].num < num) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return lo;
}

prgline* getCachedLabel(short num) {
    short i = findLabel(num);
    return labelCache[i].num == num ? (prgline*)(void*)(prgStore + labelCache[i].offset) : NULL;
}

void addCachedLabel(short num, short offset) {
    short idx = findLabel(num);
    if (idx < labelsCached) {
        memmove(labelCache + idx + 1, labelCache + idx, sizeof(labelCacheElem) * (labelsCached - idx));
    }
    labelCache[idx].num = num;
    labelCache[idx].offset = offset;
    labelsCached += 1;
}

static void advance(void) {
    if (curTok->type != TT_NONE) {
        curTok = nextToken(curTok);
    }
}

void calcOperation(char op) {
    numeric top = calcStack[sp++];
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
    numeric i;
    if (cmpNStrToStr(name, "ABS")) {
        calcStack[sp] = abs(calcStack[sp]);
        return;
    }
    for (i = 0; extraFuncs[i][0]; i++) {
        if (cmpNStrToStr(name, extraFuncs[i])) {
            i = extraFunction(i, calcStack + sp);
            sp += extraFuncArgCnt[i] - 1;
            calcStack[sp] = i;
            return;
        }
    }
    calcStack[sp] = 0;
}

numeric calcExpression(void) {
    while (1) {
        switch (curTok->type) {
            case TT_NONE:
            case TT_SEPARATOR:
                return calcStack[sp++];
            case TT_NUMBER:
                calcStack[--sp] = curTok->body.integer;
                break;
            case TT_VARIABLE:
                calcStack[--sp] = getVar(shortVarName(&(curTok->body.str)));
                break;
            case TT_SYMBOL:
                calcOperation(curTok->body.symbol);
                break;
            case TT_FUNCTION:
                calcFunction(&(curTok->body.str));
                break;
        }
        advance();
    }
}

void execAssignment(void) {
    short varname = shortVarName(&(curTok->body.str));
    advance();
    advance();
    setVar(varname, calcExpression());
}

void execRem(void) {
    while (curTok->type != TT_NONE) {
        advance();
    }
}

void execPrint(void) {
    while (1) {
        switch (curTok->type) {
            case TT_NONE:
                outputCr();
                return;
            case TT_SEPARATOR:
                break;
            case TT_LITERAL:
                outputNStr(&(curTok->body.str));
                break;
            default:
                outputInt(calcExpression());
                break;
        }
        advance();
    }
}

void execInput(void) {
    char s[16];
    while (1) {
        switch (curTok->type) {
            case TT_NONE:
                return;
            case TT_SEPARATOR:
                break;
            case TT_VARIABLE:
                outputStr("? ");
                input(s, sizeof(s));
                setVar(shortVarName(&(curTok->body.str)), decFromStr(s));
                break;
        }
        advance();
    }
}

void execIf(void) {
    if (calcExpression() == 0) {
        while (curTok->type != TT_NONE) {
            advance();
        }
    } else {
        advance();
    }
}

void execGoto(void) {
    nextLineNum = curTok->body.integer;
    advance();
}

void execGosub(void) {
    calcStack[--sp] = nextLineNum;
    nextLineNum = curTok->body.integer;
    advance();
}

void execReturn(void) {
    nextLineNum = calcStack[sp++];
}

void execEnd(void) {
    nextLineNum = 32767;
}

void execExtra(char cmd) {
    char n = extraCmdArgCnt[cmd];
    char i;
    for (i = 0; i < n; i++) {
        calcStack[sp - n + i] = calcExpression();
        advance();
    }
    extraCommand(cmd, calcStack + (sp - n));
}

char executeTokens(token* t) {
    curTok = t;
    while (t->type != TT_NONE) {
        if (t->type == TT_COMMAND) {
            advance();
            if (t->body.command < CMD_EXTRA) {
                executors[t->body.command]();
            } else {
                execExtra(t->body.command - CMD_EXTRA);
            }
        } else {
            execAssignment();
        }
        t = curTok;
    }
    return 1;
}

void signalEndOfCode(void) {
    outputStr("End of code\n");
}

char executeStep(char* lineBuf, token* tokenBuf) {
    prgline* p = findLine(nextLineNum);
    if (p->num == 0) {
        signalEndOfCode();
        return 1;
    }
    nextLineNum = p->num + 1;
    memcpy(lineBuf, p->str.text, p->str.len);
    lineBuf[p->str.len] = 0;
    parseLine(lineBuf, tokenBuf);
    executeTokens(tokenBuf);
    return 0;
}

void executeParsedRun(void) {
    prgline* p = findLine(nextLineNum);
    prgline* next;
    labelsCached = 0;
    labelCache = (labelCacheElem*)(void*)(prgStore + prgSize);
    while (1) {
        if (p->num == 0 || nextLineNum == 0) {
            return;
        }
        next = (prgline*)(void*)((char*)(void*)p + sizeof(p->num) + sizeof(p->str.len) + p->str.len);
        nextLineNum = next->num;
        executeTokens((token*)(void*)(p->str.text));
        if (next->num != nextLineNum) {
            p = getCachedLabel(nextLineNum);
            if (p == NULL) {
                p = findLine(nextLineNum);
                addCachedLabel(nextLineNum, (short)((char*)(void*)p - (char*)(void*)prgStore));
            }
        } else {
            p = next;
        }
    }
    signalEndOfCode();
}

