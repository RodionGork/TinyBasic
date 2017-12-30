#include "mystdlib.h"
#include "tokens.h"
#include "editor.h"
#include "exectoks.h"
#include "utils.h"
#include "extern.h"
#include "textual.h"

short listLine, listPage;

#if 0
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
        case TT_ARRAY:
            outputStr("ARR=");
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

void printTokens(token* t) {
    while (1) {
        printToken(t);
        outputChar(' ');
        if (tokenClass(t) == TT_NONE) {
            break;
        }
        t = nextToken(t);
    }
    outputCr();
}
#else
void printTokens(token* t) {
}
#endif

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

void executeSteps(char* lineBody, token* tokensBody) {
    token* t = nextToken(nextToken(tokensBody));
    executeNonParsed(lineBody, tokensBody, t->type == TT_NUMBER ? t->body.integer : 1);
}

void executeRun(char* lineBody, token* tokensBody) {
    nextLineNum = 1;
    if (editorSave()) {
        editorLoadParsed(lineBody, tokensBody);
        executeParsedRun();
        editorLoad();
    } else {
        executeNonParsed(lineBody, tokensBody, -1);
    }
}

void prgReset(void) {
    resetEditor();
    resetTokenExecutor();
}

void showInfo(void) {
    outputConstStr(ID_COMMON_STRINGS, 1, NULL); // code:
    outputInt(prgSize);
    outputCr();
    outputConstStr(ID_COMMON_STRINGS, 2, NULL); // vars:
    outputInt(varSize());
    outputCr();
    outputConstStr(ID_COMMON_STRINGS, 3, NULL); // next:
    outputInt(nextLineNum);
    outputCr();
}

void metaOrError(token* t, char* line) {
    numeric h = tokenHash(t);
    if (h == 0x31A) { // QUIT
        sysQuit();
    } else if (h == 0x3B6) { // LIST
        listProgram(t);
    } else if (h == 0x312) { // STEP
        executeSteps(line, t);
    } else if (h == 0x1AC) { // RUN
        executeRun(line, t);
    } else if (h == 0x375) { // SAVE
        editorSave();
    } else if (h == 0x39A) { // LOAD
        editorLoad();
    } else if (h == 0x69A) { // RESET
        prgReset();
    } else if (h == 0x3B3) { // INFO
        showInfo();
    } else {
        getParseErrorMsg(line);
        outputStr(line);
        outputChar(' ');
        outputInt((long)(getParseErrorPos() - line) + 1);
        outputCr();
    }
}

void processLine(char* line, token* t) {
    if (line[0] == 0) {
        return;
    }
    parseLine(line, t);
    printTokens(t);
    if (getParseErrorPos() != NULL) {
        metaOrError(t, line);
        return;
    }
    if (t->type != TT_NUMBER) {
        executeTokens(t);
    } else {
        injectLine(skipSpaces(skipDigits(line)), t->body.integer);
    }
}

void init(char* space, short dataSize) {
    outputCr();
    outputConstStr(ID_COMMON_STRINGS, 0, NULL); // Miskatino vX.X
    outputCr();
    outputCr();
    initEditor(space + dataSize);
    initTokenExecutor(space, dataSize);
    listLine = 1;
    listPage = 3;
}

void preload(char* line, token* t) {
    if (editorLoadParsed(line, t)) {
        sysDelay(1000);
        if (sysGetc() < 0) {
            executeParsedRun();
        }
    }
    prgReset();
}

void dispatch(void) {
    char line[MAX_LINE_LEN];
    char toksBody[MAX_LINE_LEN * 2];
    token* t = (token*)(void*) toksBody;
    preload(line, t);
    while (1) {
        readLine(line);
        processLine(line, t);
    }
}

