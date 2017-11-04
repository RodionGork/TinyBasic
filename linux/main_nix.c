#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>

#include "../core/main.h"
#include "../core/utils.h"

char* extraCmds[] = {
    "POKE",
    "DELAY",
    "",
};

char extraCmdArgCnt[] = {2, 1};

char* extraFuncs[] = {
    "PEEK",
    "",
};

char extraFuncArgCnt[] = {1};

char dataSpace[4096];

FILE* fCurrent;
short idCurrent = 0;
volatile char interrupted;

struct termios oldTermSettings;

void initSystem(void) {
    struct termios termSettings;
    tcgetattr(STDIN_FILENO, &oldTermSettings);
    termSettings = oldTermSettings;
    termSettings.c_lflag &= ~(ICANON | ECHO | ISIG);
    tcsetattr(STDIN_FILENO, TCSANOW, &termSettings);
    setvbuf(stdout, NULL, _IONBF, 0);
}

void cleanup(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldTermSettings);
}

short sysGetc(void) {
    struct pollfd fd;
    fd.fd = STDIN_FILENO;
    fd.events = POLLIN;
    if (!poll(&fd, 1, 0)) {
        return -1;
    }
    return getchar();
}

void sysPutc(char c) {
    putchar(c);
}

void sysEcho(char c) {
    sysPutc(c);
}

char sysBreak(char v) {
    if (v == 0) {
        interrupted = 0;
        return 0;
    }
    return interrupted;
}

void sysQuit(void) {
    cleanup();
    exit(0);
}

void sysPoke(unsigned long addr, uchar value) {
    dataSpace[addr] = value;
}

uchar sysPeek(unsigned long addr) {
    return dataSpace[addr];
}

void sysDelay(numeric pause) {
    usleep(pause * 1000L);
}

void extraCommand(char cmd, numeric args[]) {
    switch (cmd) {
        case 0:
            sysPoke(args[0], args[1]);
            break;
        case 1:
            sysDelay(args[0]);
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
    initSystem();
    init(dataSpace, 512);
    dispatch();
    return 0;
}

