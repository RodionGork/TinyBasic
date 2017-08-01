#include <stdio.h>
#include <stdlib.h>

#include "../core/main.h"
#include "../core/utils.h"

char* extraCmds[] = {
    "POKE",
    "",
};

char extraCmdArgCnt[] = {2};

char* extraFuncs[] = {
    "PEEK",
    "",
};

char extraFuncArgCnt[] = {1};

char dataSpace[4096];

FILE* fCurrent;
short idCurrent = 0;

short sysGetc(void) {
    return getc(stdin);
}

void sysPutc(char c) {
    putc(c, stdout);
}

void sysEcho(char c) {
}

void sysQuit(void) {
    exit(0);
}

void sysPoke(unsigned long addr, uchar value) {
    dataSpace[addr] = value;
}

uchar sysPeek(unsigned long addr) {
    return dataSpace[addr];
}

void extraCommand(char cmd, numeric args[]) {
    switch (cmd) {
        case 0:
            sysPoke(args[0], args[1]);
            break;
    }
}

numeric extraFunction(char cmd, numeric args[]) {
    switch (cmd) {
        case 0:
            return sysPeek(args[0]);
    }
    return 0;
}

FILE* openStorage(char id, char op) {
    char fname[] = "store0.dat";
    char ops[] = "xb";
    fname[5] += id;
    ops[0] = op;
    return fopen(fname, ops);
}

char storageOperation(void* data, short size) {
    if (data == NULL) {
        if (idCurrent != 0) {
            fclose(fCurrent);
        }
        idCurrent = 0;
        if (size != 0) {
            idCurrent = abs(size);
            fCurrent = openStorage(idCurrent, size > 0 ? 'w' : 'r');
        }
        return 1;
    }
    if (size > 0) {
        fwrite(data, size, 1, fCurrent);
    } else {
        fread(data, -size, 1, fCurrent);
    }
    return 1;
}

int main(void) {
    init(dataSpace, 512);
    dispatch();
    return 0;
}

