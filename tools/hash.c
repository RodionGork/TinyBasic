#include <stdio.h>

int hash(char* s) {
    int res = 0;
    while (1) {
        res ^= *s;
        s++;
        if (*s == 0) {
            return res;
        }
        res <<= 1;
    }
}

int main(void) {
    char w[128];
    while (scanf("%s", w)) {
        printf("%s: %04X\n", w, hash(w));
    }
    return 0;
}

