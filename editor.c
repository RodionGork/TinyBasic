#include <string.h>

#include "editor.h"
#include "utils.h"

char* prgStore;
short prgSize;

void resetEditor(void) {
    ((prgline*)prgStore)->num = 0;
    prgSize = 2;
}

void initEditor(char* prgBody) {
    prgStore = prgBody;
    resetEditor();
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
    prgline* p = (prgline*)(void*)prgStore;
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
        memmove(p, nextLine(p), prgStore + prgSize - (char*)(void*)nextLine(p));
        prgSize -= len;
    }
    len = strlen(s);
    if (len > 0) {
        memmove((char*)(void*)p + len + 3, p, prgStore + prgSize - (char*)(void*)p);
        prgSize += len + 3;
        p->num = num;
        p->str.len = len;
        memcpy(p->str.text, s, len);
    }
}

void editorSave(void) {
    storageOperation(NULL, 1);
    storageOperation(&prgSize, sizeof(prgSize));
    storageOperation(prgStore, prgSize);
    storageOperation(NULL, 0);
}

void editorLoad(void) {
    storageOperation(NULL, -1);
    storageOperation(&prgSize, (short)-sizeof(prgSize));
    storageOperation(prgStore, -prgSize);
    storageOperation(NULL, 0);
}

