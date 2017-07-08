#include <stdio.h>

#include "main.h"

short sysgetc(void) {
    return getc(stdin);
}

void sysputc(char c) {
    putc(c, stdout);
}

short adcread(char channel) {
    return 0;
}

char pinread(char pin) {
    return 0;
}

void pinout(char pin, char state) {
}

int main(void) {
    char dataSpace[4096];
    init(dataSpace, 512);
    dispatch();
    return 0;
}

