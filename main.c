#include <string.h>
#include <ctype.h>

#include "tokens.h"
#include "editor.h"
#include "exectoks.h"
#include "utils.h"

short listLine, listPage;

void printToken(token* t) {
    switch (t->type) {
        case TT_NUMBER:
            outputStr("INT=");
            outputInt(t->body.integer);
            break;
        case TT_NAME:
            outputStr("NAME=");
            outputNStr(&(t->body.str));
            break;
        case TT_VARIABLE:
            outputStr("VAR=");
            outputNStr(&(t->body.str));
            break;
        case TT_FUNCTION:
            outputStr("FN=");
            outputNStr(&(t->body.str));
            break;
        case TT_COMMAND:
            outputStr("CMD=");
            outputInt(t->body.command);
            break;
        case TT_LITERAL:
            outputStr("STR=\"");
            outputNStr(&(t->body.str));
            outputStr("\"");
            break;
        case TT_COMMENT:
            outputStr("REM=\"");
            outputNStr(&(t->body.str));
            outputStr("\"");
            break;
        case TT_SYMBOL:
            outputStr("SYM=");
            outputChar(t->body.symbol);
            break;
        case TT_FUNC_END:
            outputStr("FE=%d");
            outputInt(t->body.symbol);
            break;
        case TT_NONE:
            outputChar('N');
            break;
        case TT_SEPARATOR:
            outputChar(';');
            break;
        default:
            outputChar('E');
            break;
    }
}

void printTokens(token* toks) {
    void* t = toks;
    while (1) {
        printToken(t);
        outputChar(' ');
        if (tokenClass(t) == TT_NONE) {
            break;
        }
        t += tokenSize(t);
    }
    outputCr();
}

void printProgram(void) {
    prgline* p = findLine(listLine);
    if (p->num == 0 && listLine > 1) {
        p = findLine(1);
    }
    short lineCount = 0;
    while (p->num != 0 && lineCount < listPage) {
        listLine = p->num + 1;
        outputInt(p->num);
        outputChar(' ');
        outputNStr(&(p->str));
        outputCr();
        p = findLine(p->num + 1);
        lineCount += 1;
    }
}

void listProgram(token* t) {
    t = nextToken(nextToken(t));
    if (t->type == TT_NUMBER) {
        listLine = t->body.integer;
        t = nextToken(t);
        if (t->type == TT_NUMBER) {
            listPage = t->body.integer;
        }
    }
    printProgram();
}

int metaOrError(token* t, char* line) {
    if (tokenNameEqual(t, "QUIT")) {
        return 1;
    } else if (tokenNameEqual(t, "LIST")) {
        listProgram(t);
    } else if (tokenNameEqual(t, "STEP")) {
        executeStep(line, t);
    } else {
        outputStr(getParseErrorMsg());
        outputStr(" (");
        outputInt((int)(getParseErrorPos() - line) + 1);
        outputStr(")\n");
    }
    return 0;
}

int processLine(char* line) {
    char toksBody[MAX_LINE_LEN * 2];
    token* t = (void*) toksBody;
    parseLine(line, t);
    //printTokens(t);
    if (getParseErrorPos() != NULL) {
        return metaOrError(t, line);
    }
    if (t->type != TT_NUMBER) {
        executeTokens(t);
    } else {
        injectLine(skipSpaces(skipDigits(line)), t->body.integer);
    }
    return 0;
}

void init(void* space, int dataSize) {
    outputStr("\nTinyBasic 0.1-PoC\n\n");
    initEditor(space + dataSize);
    initTokenExecutor(space, dataSize);
    listLine = 1;
    listPage = 3;
}

void dispatch(void) {
    char line[MAX_LINE_LEN];
    int quit = 0;
    while (!quit) {
        if (!readLine(line)) {
            break;
        }
        quit = processLine(line);
    }
}

int main(void) {
    char dataSpace[4096];
    init(dataSpace, 512);
    dispatch();
    return 0;
}
