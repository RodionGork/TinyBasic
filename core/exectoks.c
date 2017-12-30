#include "mystdlib.h"
#include "mytypes.h"
#include "tokens.h"
#include "tokenint.h"
#include "editor.h"
#include "utils.h"
#include "extern.h"
#include "textual.h"

#define BREAK_DETECTED() (lastInput == 3)

numeric* calcStack;
short nextLineNum = 1;
short sp, spInit;
varHolder* vars;
char numVars;
short arrayBytes;
labelCacheElem* labelCache;
short labelsCached;
short lastInput;
numeric lastDim;

void execRem(void);
void execPrint(void);
void execInput(void);
void execIf(void);
void execGoto(void);
void execGosub(void);
void execReturn(void);
void execEnd(void);
void execLet(void);
void execLeta(void);
void execDim(void);
void execDelay(void);
void execData(void);

void (*executors[])(void) = {
    execRem,
    execPrint,
    execInput,
    execIf,
    execGoto,
    execGosub,
    execReturn,
    execEnd,
    execLet,
    execLeta,
    execDim,
    execDelay,
    execData,
};

void resetTokenExecutor(void) {
    numVars = 0;
    arrayBytes = 0;
    sp = spInit;
    vars[0].name = 0;
}

short varSize(void) {
    return numVars * sizeof(varHolder) + arrayBytes;
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

short shortArrayName(char letter) {
    return 0x7F00 | letter;
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

short getArrayOffset(char letter) {
    short name = shortArrayName(letter);
    char i = findVar(name);
    return (vars[i].name == name) ? vars[i].value : -1;
}

void setVar(short name, numeric value) {
    char i = findVar(name);
    if (vars[i].name != name) {
        if (i < numVars) {
            memmove(vars + i + 1, vars + i, sizeof(varHolder) * (numVars - i) + arrayBytes);
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
    short i;
    numeric r;
    numeric h = hashOfNStr(name);
    if (h == 0x1FF) { // KEY
        i = calcStack[sp];
        calcStack[sp] = lastInput;
        if (i != 0 && !BREAK_DETECTED()) {
            lastInput = -1;
        }
        return;
    }
    if (h == 0x1D3) { // ABS
        if (calcStack[sp] < 0) {
            calcStack[sp] = -calcStack[sp];
        }
        return;
    }
    i = extraFunctionByHash(h);
    if (i >= 0) {
        // arguments will appear in reverse order
        r = extraFunction(i, calcStack + sp);
        sp += extraFuncArgCnt[i] - 1;
        calcStack[sp] = r;
        return;
    }
    calcStack[sp] = 0;
}

void calcArray(char letter) {
    short offset = getArrayOffset(letter);
    if (offset == -1) {
        calcStack[sp] = 0;
        return;
    }
    char b = (offset & 0x8000) ? 1 : sizeof(numeric);
    offset = (offset & 0x7FFF) + b * calcStack[sp];
    char* p = ((char*)(void*)vars) + sizeof(varHolder) * numVars + offset;
    if (b > 1) {
        calcStack[sp] = *((numeric*)(void*)p);
    } else {
        calcStack[sp] = *((unsigned char*)(void*)p);
    }
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
            case TT_ARRAY:
                calcArray(curTok->body.symbol);
                break;
        }
        advance();
    }
}

void execLet(void) {
    short varname = shortVarName(&(curTok->body.str));
    advance();
    setVar(varname, calcExpression());
}

void setArray(char symbol, short idx, numeric value) {
    short offset = getArrayOffset(symbol);
    if (offset == -1) {
        return;
    }
    char b = (offset & 0x8000) ? 1 : sizeof(numeric);
    offset = (offset & 0x7FFF) + b * idx;
    char* p = ((char*)(void*)vars) + sizeof(varHolder) * numVars + offset;
    if (b > 1) {
        *((numeric*)(void*)p) = value;
    } else {
        *((unsigned char*)(void*)p) = (value & 0xFF);
    }
}

void execLeta(void) {
    char a = curTok->body.symbol;
    advance();
    short idx = calcExpression();
    advance();
    setArray(a, idx, calcExpression());
}

void execDim(void) {
    short name = shortArrayName(curTok->body.symbol);
    lastDim = curTok->body.symbol & 0x1F;
    advance();
    short len = curTok->body.integer;
    advance();
    char itemSize;
    if (curTok->type == TT_NONE) {
        itemSize = sizeof(numeric);
    } else {
        advance();
        itemSize = 1;
    }
    char pos = findVar(name);
    if (vars[pos].name == name) {
        return;
    }
    setVar(name, arrayBytes | (itemSize == 1 ? 0x8000 : 0));
    arrayBytes += len * itemSize;
}

void execData(void) {
    char a = (lastDim & 0x1F) | 0x40; // capital letter
    if (a < 'A' || a > 'Z') {
        return;
    }
    do {
        setArray(a, lastDim >> 5, curTok->body.integer);
        advance();
        lastDim += (1 << 5);
    } while (curTok->type != TT_NONE);
}

void execDelay(void) {
    sysDelay(calcExpression());
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
                outputChar('?');
                outputChar(' ');
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
    sp -= n;
    for (i = 0; i < n; i++) {
        calcStack[sp + i] = calcExpression();
        advance();
    }
    extraCommand(cmd, calcStack + sp);
    sp += n;
}

void fetchLastInput(void) {
    short c = sysGetc();
    if (lastInput == -1 || c == 3) {
        lastInput = c;
    }
}

char executeTokens(token* t) {
    fetchLastInput();
    curTok = t;
    while (t->type != TT_NONE) {
        advance();
        if (t->body.command < CMD_EXTRA) {
            executors[t->body.command]();
        } else {
            execExtra(t->body.command - CMD_EXTRA);
        }
        t = curTok;
    }
    return 1;
}

void signalEndOfCode(void) {
    outputConstStr(ID_COMMON_STRINGS, 5, NULL);
    outputCr();
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

void resetLastInput() {
    lastInput = -1;
}

void execBreak() {
    outputConstStr(ID_COMMON_STRINGS, 4, NULL); // BREAK
    outputCr();
    sp = spInit;
    resetLastInput();
}

void executeNonParsed(char* lineBuf, token* tokenBuf, numeric count) {
    resetLastInput();
    while (count != 0) {
        if (executeStep(lineBuf, tokenBuf)) {
            break;
        }
        if (BREAK_DETECTED()) {
            execBreak();
            break;
        }
        if (count != -1) {
            count -= 1;
        }
    }
}

void executeParsedRun(void) {
    prgline* p = findLine(nextLineNum);
    prgline* next;
    labelsCached = 0;
    labelCache = (labelCacheElem*)(void*)(prgStore + prgSize);
    resetLastInput();
    while (1) {
        if (p->num == 0 || nextLineNum == 0) {
            break;
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
        if (BREAK_DETECTED()) {
            execBreak();
            return;
        }
    }
    signalEndOfCode();
}

