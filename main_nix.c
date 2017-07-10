#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "utils.h"

short sysgetc(void) {
    return getc(stdin);
}

void sysputc(char c) {
    putc(c, stdout);
}

short adcread(char channel) {
    return 0;
}

char pinread(char pin) {
    return 0;
}

void pinout(char pin, char state) {
    outputStr("pinout: ");
    outputInt(pin);
    outputChar(',');
    outputInt(state);
    outputCr();
}

void sysdelay(short ms) {
    outputStr("delay: ");
    outputInt(ms);
    outputCr();
}

void syssave(char id, char* data, short size) {
    char fname[] = "store0.dat";
    fname[5] += id;
    FILE* f = fopen(fname, "wb");
    fwrite(data, size, 1, f);
    fclose(f);
}

short sysload(char id, char* data) {
    char fname[] = "store0.dat";
    long size;
    fname[5] += id;
    FILE* f = fopen(fname, "rb");
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    fread(data, size, 1, f);
    fclose(f);
    return size;
}

void sysquit(void) {
    exit(0);
}

int main(void) {
    char dataSpace[4096];
    init(dataSpace, 512);
    dispatch();
    return 0;
}

