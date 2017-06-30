#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "tokens.h"
#include "editor.h"
#include "utils.h"

#define MAX_LINE_LEN 80

char line[MAX_LINE_LEN];
char toksBody[MAX_LINE_LEN * 2];

void* toks = toksBody;

void readLine(void) {
    fgets(line, sizeof(line), stdin);
    trim(line);
}

void printNStr(nstring* t) {
    for (short i = 0; i < t->len; i++) {
        putc(t->text[i], stdout);
    }
};

void printToken(token* t) {
    switch (t->type) {
        case TT_NUMBER:
            printf("{INT %d}", t->body.integer);
            break;
        case TT_NAME:
            printf("{NAME \"");
            printNStr(&(t->body.str));
            printf("\"}");
            break;
        case TT_LITERAL:
            printf("{STR \"");
            printNStr(&(t->body.str));
            printf("\"}");
            break;
        case TT_SYMBOL:
            printf("{SYM '%c'}", t->body.symbol);
            break;
        case TT_NONE:
            printf("{NONE}");
            break;
        default:
            printf("{ERROR}");
    }
}

void printTokens(void) {
    void* t = toks;
    while (1) {
        printToken(t);
        printf(" ");
        if (tokenClass(t) == TT_NONE) {
            break;
        }
        t += tokenSize(t);
    }
    printf("\n");
}

void printProgram() {
    prgline* p = findLine(1);
    while (p->num != 0) {
        printf("%d ", p->num);
        printNStr(&(p->str));
        printf("\n");
        p = findLine(p->num + 1);
    }
}

int processLine(void) {
    token* t = toks;
    parseLine(line, toks);
    if (getParseError() != NULL) {
        printf("Error at pos: %d\n", (int) (getParseError() - line) + 1);
        return 0;
    }
    if (t->type == TT_NUMBER) {
        injectLine(skipSpaces(skipDigits(line)), t->body.integer);
    } else if (tokenNameEqual(t, "QUIT")) {
        return 1;
    } else if (tokenNameEqual(t, "LIST")) {
        printProgram();
    }
    return 0;
}

void init(void) {
    printf("\nTinyBasic 0.1-PoC\n\n");
    initEditor();
}

void dispatch(void) {
    int quit = 0;
    while (!quit) {
        readLine();
        quit = processLine();
    }
}

int main(void) {
    init();
    dispatch();
    return 0;
}
