#include <stdio.h>

int sysgetc(void) {
    return getc(stdin);
}

void sysputc(char c) {
    putc(c, stdout);
}

