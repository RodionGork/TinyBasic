#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>

void tick(int c);
void initCore();

static struct termios oldTermSettings;

static void initSystem(void) {
    struct termios termSettings;
    tcgetattr(STDIN_FILENO, &oldTermSettings);
    termSettings = oldTermSettings;
    termSettings.c_lflag &= ~(ICANON | ECHO | ISIG);
    tcsetattr(STDIN_FILENO, TCSANOW, &termSettings);
    setvbuf(stdout, NULL, _IONBF, 0);
}

static void cleanup(void) {
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

void sysQuit(void) {
    cleanup();
    exit(0);
}

int sysMs(void) {
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return (((long) tp.tv_sec) * 1000 + tp.tv_nsec / 1000000) & 0x7FFFFFFF;
}

int translate(int c) {
    switch (c) {
        case '\n':
            return '\r';
        case 0x7F:
            return '\b';
        default:
            return c;
    }
}

void dispatch() {
    while (1) {
        int c = sysGetc();
        c = translate(c);
        tick(c);
    }
}

int main(void) {
    initSystem();
    initCore();
    dispatch();
    return 0;
}

