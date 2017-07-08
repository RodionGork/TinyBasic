#ifndef __EXECTOKS_H_
#define __EXECTOKS_H_

#include "tokens.h"

void initTokenExecutor(char* space, short size);
char executeTokens(token* t);
char executeStep(char* lineBuf, token* tokenBuf);

#endif

