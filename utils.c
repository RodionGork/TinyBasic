#include <ctype.h>
#include <stdio.h>

#include "utils.h"

void trim(char* s) {
    int i = 0, k;
    while (s[i] != 0 && s[i] <= ' ') {
        i++;
    }
    k = 0;
    while (s[i] != 0) {
        s[k++] = s[i++];
    }
    do {
        s[k--] = 0;
    } while (k >= 0 && s[k] <= ' ');
}

char* skipSpaces(char* s) {
    while (isspace(*s)) {
        s++;
    }
    return s;
}

char* skipDigits(char* s) {
    while (isdigit(*s)) {
        s++;
    }
    return s;
}

int charInStr(char c, char* s) {
    while (*s != 0) {
        if (*s == c) {
            return 1;
        }
        s++;
    }
    return 0;
}

int input(char* s, int n) {
    return fgets(s, n, stdin) != NULL;
}

void outputStr(char* s) {
    printf("%s", s);
}

void outputNStr(nstring* t) {
    for (short i = 0; i < t->len; i++) {
        putc(t->text[i], stdout);
    }
};

void outputInt(int n) {
    printf("%d", n);
}

void outputCr() {
    printf("\n");
}

