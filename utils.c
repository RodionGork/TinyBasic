#include <stdlib.h>
#include <ctype.h>

#include "utils.h"

void trim(char* s) {
    short i = 0, k;
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

char charInStr(char c, char* s) {
    while (*s != 0) {
        if (*s == c) {
            return 1;
        }
        s++;
    }
    return 0;
}

char input(char* s, short n) {
    short i = 0;
    short c;
    n -= 1;
    while (i < n) {
        c = sysgetc();
        if (c < 0) {
            sysdelay(1);
            continue;
        }
        if (c == '\r' || c == '\n') {
            break;
        }
        s[i++] = c;
    }
    s[i] = 0;
    return 1;
}

void outputChar(char c) {
    sysputc(c);
}

void outputStr(char* s) {
    while (*s) {
        sysputc(*(s++));
    }
}

void outputNStr(nstring* t) {
    for (short i = 0; i < t->len; i++) {
        sysputc(t->text[i]);
    }
};

void outputInt(long n) {
    long d = 1000000000;
    if (n < 0) {
        outputChar('-');
        n = -n;
    }
    if (n == 0) {
        sysputc('0');
        return;
    }
    while (d > n) {
        d /= 10;
    }
    while (d > 0) {
        sysputc(n / d + '0');
        n %= d;
        d /= 10;
    }
}

void outputCr() {
    sysputc('\n');
}

