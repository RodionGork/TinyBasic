#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>

#include "../core/main.h"
#include "../core/utils.h"
#include "../core/textual.h"
#include "../core/tokens.h"

char extraCmdArgCnt[] = {2};

char extraFuncArgCnt[] = {1};

static char* commonStrings = CONST_COMMON_STRINGS;
static char * parsingErrors = CONST_PARSING_ERRORS;

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

void outputConstStr(char strId, char index, char* w) {
    char* s;
    switch (strId) {
        case ID_COMMON_STRINGS:
            s = commonStrings;
            break;
        case ID_PARSING_ERRORS:
            s = parsingErrors;
            break;
        default:
            return;
    }
    while (index > 0) {
        while (*s++ != '\n') {
        }
        index -= 1;
    }
    while (*s != '\n') {
        if (w) {
            *(w++) = (*s++);
        } else {
            sysPutc(*s++);
        }
    }
    if (w) {
        *w = 0;
    }
}

short extraCommandByHash(numeric h) {
    switch (h) {
        case 0x036F: // POKE
            return CMD_EXTRA + 0;
        default:
            return -1;
    }
}

short extraFunctionByHash(numeric h) {
    switch (h) {
        case 0x0355: // PEEK
            return 0;
        default:
            return -1;
    }
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

char openStorage(char id, char op) {
    char fname[] = "store0.dat";
    char ops[] = "xb";
    fname[5] += id;
    ops[0] = op;
    fCurrent = fopen(fname, ops);
    return fCurrent != NULL ? 1 : 0;
}

char storageOperation(void* data, short size) {
    if (data == NULL) {
        if (idCurrent != 0) {
            fclose(fCurrent);
        }
        idCurrent = 0;
        if (size != 0) {
            idCurrent = abs(size);
            if (!openStorage(idCurrent, size > 0 ? 'w' : 'r')) {
                idCurrent = 0;
                return 0;
            }
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

