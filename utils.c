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
        c = sysGetc();
        if (c < 0) {
            sysDelay(1);
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
    sysPutc(c);
}

void outputStr(char* s) {
    while (*s) {
        sysPutc(*(s++));
    }
}

void outputNStr(nstring* t) {
    for (short i = 0; i < t->len; i++) {
        sysPutc(t->text[i]);
    }
};

void outputInt(long n) {
    long d = 1000000000;
    if (n < 0) {
        outputChar('-');
        n = -n;
    }
    if (n == 0) {
        sysPutc('0');
        return;
    }
    while (d > n) {
        d /= 10;
    }
    while (d > 0) {
        sysPutc(n / d + '0');
        n %= d;
        d /= 10;
    }
}

void outputCr() {
    sysPutc('\n');
}

