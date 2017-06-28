#include <stdio.h>
#include <string.h>

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

int processLine(void) {
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
