#ifndef __EDITOR_H_
#define __EDITOR_H_

#include "utils.h"

#define MAX_LINE_LEN 80

typedef struct prgline {
    short num;
    nstring str;
} __attribute__((packed)) prgline;

void initEditor(char* prgBody);
char readLine(char* line);
prgline* findLine(short num);
void injectLine(char* s, short num);

#endif

