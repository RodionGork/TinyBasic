#include "mystdlib.h"
#include "tokens.h"
#include "editor.h"
#include "exectoks.h"
#include "utils.h"
#include "extern.h"

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
    outputStr("code: ");
    outputInt(prgSize);
    outputCr();
    outputStr("vars: ");
    outputInt(varSize());
    outputCr();
    outputStr("next: ");
    outputInt(nextLineNum);
    outputCr();
}

void showHelp(void) {
}

void metaOrError(token* t, char* line) {
    if (tokenNameEqual(t, "QUIT")) {
        sysQuit();
    } else if (tokenNameEqual(t, "LIST")) {
        listProgram(t);
    } else if (tokenNameEqual(t, "STEP")) {
        executeSteps(line, t);
    } else if (tokenNameEqual(t, "RUN")) {
        executeRun(line, t);
    } else if (tokenNameEqual(t, "SAVE")) {
        editorSave();
    } else if (tokenNameEqual(t, "LOAD")) {
        editorLoad();
    } else if (tokenNameEqual(t, "RESET")) {
        prgReset();
    } else if (tokenNameEqual(t, "INFO")) {
        showInfo();
    } else if (tokenNameEqual(t, "HELP")) {
        showHelp();
    } else {
        getParseErrorMsg(line);
        outputStr(line);
        outputChar(' ');
        outputInt((long)(getParseErrorPos() - line) + 1);
        outputCr();
    }
}

void processLine(char* line) {
    char toksBody[MAX_LINE_LEN * 2];
    token* t = (token*)(void*) toksBody;
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
    outputStr("Miskatino 1.1");
    outputCr();
    outputCr();
    initEditor(space + dataSize);
    initTokenExecutor(space, dataSize);
    listLine = 1;
    listPage = 3;
}

void dispatch(void) {
    char line[MAX_LINE_LEN];
    while (1) {
        readLine(line);
        processLine(line);
    }
}

