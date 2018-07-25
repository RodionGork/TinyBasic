#include <stdio.h>
#include <emscripten.h>

void tick(int c);
void initCore();

void sysPutc(char c) {
    char t[128];
    sprintf(t, "tb.putc('%d')", (int) c);
    emscripten_run_script(t);
}

void sysQuit(void) {
}

int sysMs(void) {
    return 1;
}

