#include <emscripten.h>
#include <stdio.h>

extern "C" {

short sqr(short x) {
    return x*x;
}

void notify(char* s) {
    char t[128];
    sprintf(t, "alert('%s')", s);
    emscripten_run_script(t);
}

}