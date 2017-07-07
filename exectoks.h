#ifndef __EXECTOKS_H_
#define __EXECTOKS_H_

#include "tokens.h"

void initTokenExecutor(void* space, int size);
int executeTokens(token* t);
int executeStep(char* lineBuf, token* tokenBuf);

#endif

