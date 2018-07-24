#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STATE_IA 0x00
#define STATE_RUN 0x40
#define STATE_INPUT 1
#define STATE_DELAY 2

char inBuf[128];
char prg[16 * 36];
int vars[26];
int cur;

int state;
int next;
int lastKey;
long delayLimit, delayT0;

void sysPutc(char c);
void sysQuit();
int sysMs();

int varNum(int idx) {
    return toupper(inBuf[idx]) - 'A';
}

int getVar(int idx) {
    return vars[varNum(idx)];
}

void assignVal() {
    vars[varNum(1)] = atoi(inBuf + 2);
}

void arithm() {
    switch (inBuf[0]) {
        case '+':
            vars[varNum(1)] += getVar(2);
            break;
        case '-':
            vars[varNum(1)] -= getVar(2);
            break;
        case '*':
            vars[varNum(1)] *= getVar(2);
            break;
        case '/':
            vars[varNum(1)] /= getVar(2);
            break;
        case '%':
            vars[varNum(1)] %= getVar(2);
            break;
    }
}

void delay() {
    delayT0 = sysMs();
    delayLimit = atoi(inBuf + 1);
    state |= STATE_DELAY;
}

void checkDelay() {
    if (sysMs() - delayT0 > delayLimit) {
        state &= ~STATE_DELAY;
    }
}

int numFromChar(int c) {
    c = toupper(c);
    return (c <= '9') ? c - '0' : c - 'A' + 10;
}

void storeLine() {
    int num = numFromChar(inBuf[1]);
    strcpy(prg + num * 16, inBuf + 2);
}

void listPrg() {
    for (int i = 0; i < 36; i++) {
        char* s = prg + i * 16;
        if (*s == 0) {
            break;
        }
        printf("%02d %s\n", i, s);
    }
}

void jump() {
    next = numFromChar(inBuf[1]);
}

void cond() {
    if (getVar(1)) {
        next = numFromChar(inBuf[2]);
    }
}

void inkey() {
    vars[varNum(1)] = ((state & STATE_RUN) != 0) ? lastKey : -1;
    lastKey = -1;
}

void run() {
    state = STATE_RUN;
}

void execCmd();

void step() {
    strcpy(inBuf, prg + next * 16);
    next += 1;
    execCmd();
}

void execCmd() {
    switch (inBuf[0]) {
        case ':':
            storeLine();
            break;
        case '=':
            assignVal();
            break;
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
            arithm();
            break;
        case 'p':
            printf("%d\n", getVar(1));
            break;
        case 'd':
            delay();
            break;
        case 't':
            printf("%d\n", sysMs());
            break;
        case 'l':
            listPrg();
            break;
        case 'j':
            jump();
            break;
        case '?':
            cond();
            break;
        case 'k':
            inkey();
            break;
        case 'r':
            run();
            break;
        default:
            printf("Bad command: %c\n", inBuf[0]);
            break;
    }
}

void initCore() {
    memset(prg, 0, sizeof(prg));
    cur = 0;
    state = STATE_IA;
    next = 0;
}

void tick(int c) {
    if (c > 0) {
        lastKey = c;
        if (c == 3) {
            sysQuit();
        }
    }
    if ((state & STATE_DELAY) != 0) {
        checkDelay();
        return;
    } else if ((state & STATE_RUN) != 0) {
        if (lastKey != 3) {
            step();
        } else {
            state &= ~STATE_RUN;
        }
        return;
    }
    if (c >= ' ') {
        inBuf[cur] = c;
        cur += 1;
        sysPutc(c);
    } else if (c == '\b') {
        if (cur > 0) {
            cur -= 1;
            sysPutc('\b');
            sysPutc(' ');
            sysPutc('\b');
        }
    } else if (c == '\r') {
        if (cur > 0) {
            inBuf[cur] = 0;
            printf("\r\n");
            execCmd();
            cur = 0;
        }
    }
}

