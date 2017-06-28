#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "tokens.h"
#include "utils.h"

#define MAX_PRG_SIZE 4096
#define MAX_LINE_LEN 80

char prg[MAX_PRG_SIZE];
char line[MAX_LINE_LEN];
char toks[MAX_LINE_LEN * 2];

void readLine(void) {
    fgets(line, sizeof(line), stdin);
    trim(line);
}


void printText(char* t) {
    for (int cnt = *t; cnt != 0; cnt--) {
        putc(*(++t), stdout);
    }
};

void printToken(token* t) {
    switch (t->type) {
        case TT_NUMBER:
            printf("{INT %d}", t->body.integer);
            break;
        case TT_NAME:
            printf("{NAME \"");
            printText(t->body.text);
            printf("\"}");
            break;
        case TT_LITERAL:
            printf("{STR \"");
            printText(t->body.text);
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
    void* t = (void*) toks;
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

int processLine(void) {
    parseLine(line, toks);
    printTokens();
    if (getParseError() != NULL) {
        printf("Error at pos: %d\n", (int) (getParseError() - line) + 1);
    }
    if (strcmp(line, "quit") == 0) {
        return 1;
    }
    return 0;
}

void init(void) {
    printf("\nTinyBasic 0.1-PoC\n\n");
    prg[0] = 0;
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
