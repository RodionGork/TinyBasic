#include <ctype.h>

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
