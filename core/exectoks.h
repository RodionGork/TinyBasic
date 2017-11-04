#ifndef __EXECTOKS_H_
#define __EXECTOKS_H_

#include "tokens.h"

extern short nextLineNum;

void resetTokenExecutor(void);
void initTokenExecutor(char* space, short size);
short varSize(void);
char executeTokens(token* t);
char executeStep(char* lineBuf, token* tokenBuf);
void execBreak(void);
void executeNonParsed(char* lineBuf, token* tokenBuf, numeric count);
void executeParsedRun(void);

#endif

