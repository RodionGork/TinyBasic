#ifndef __EDITOR_H_
#define __EDITOR_H_

#include "utils.h"

extern short prgSize;

void resetEditor(void);
void initEditor(char* prgBody);
char readLine(char* line);
prgline* findLine(short num);
void injectLine(char* s, short num);
void editorSave(void);
void editorLoad(void);

#endif

