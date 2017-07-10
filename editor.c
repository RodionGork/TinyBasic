#include <string.h>

#include "editor.h"
#include "utils.h"

char* prg;
short prgSize;

void initEditor(char* prgBody) {
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

prgline* nextLine(prgline* p) {
    return (prgline*)(void*)((char*)(void*)p + lineSize(p));
}

prgline* findLine(short num) {
    prgline* p = (prgline*)(void*)prg;
    while (p->num != 0 && p->num < num) {
        p = nextLine(p);
    }
    return p;
}

void injectLine(char* s, short num) {
    unsigned char len;
    prgline* p = findLine(num);
    if (p->num == num) {
        len = (char*)(void*)nextLine(p) - (char*)(void*)p;
        memmove(p, nextLine(p), prg + prgSize - (char*)(void*)nextLine(p));
        prgSize -= len;
    }
    len = strlen(s);
    if (len > 0) {
        memmove((char*)(void*)p + len + 3, p, prg + prgSize - (char*)(void*)p);
        prgSize += len + 3;
        p->num = num;
        p->str.len = len;
        memcpy(p->str.text, s, len);
    }
}

void editorSave(void) {
    syssave(0, prg, prgSize);
}

void editorLoad(void) {
    prgSize = sysload(0, prg);
}

