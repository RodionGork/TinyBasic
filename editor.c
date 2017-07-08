#include <string.h>

#include "editor.h"
#include "utils.h"

void* prg;
short prgSize;

void initEditor(void* prgBody) {
    prg = prgBody;
    ((prgline*)prg)->num = 0;
    prgSize = 2;
}

char readLine(char* line) {
    if (!input(line, MAX_LINE_LEN)) {
        return 0;
    }
    trim(line);
    return 1;
}

short lineSize(prgline* p) {
    return p->str.len + 3;
}

prgline* nextLine(void* p) {
    return p + lineSize(p);
}

prgline* findLine(short num) {
    prgline* p = prg;
    while (p->num != 0 && p->num < num) {
        p = nextLine(p);
    }
    return p;
}

void injectLine(char* s, short num) {
    unsigned char len = strlen(s);
    prgline* p = findLine(num);
    if (p->num == num) {
        memmove(p, nextLine(p), prg + prgSize - (void*) nextLine(p));
    }
    if (len > 0) {
        memmove(((void*)p) + len + 3, p, prg + prgSize - (void*) p);
        prgSize += len + 3;
        p->num = num;
        p->str.len = len;
        memcpy(p->str.text, s, len);
    }
}

