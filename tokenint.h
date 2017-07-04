#ifndef __TOKENINT_H_
#define __TOKENINT_H_

char* getCurTokPos();
void skipTokenInInput(int skip);
int parseName(char checkCmd);
int parseSymbol(void);
int parseNumber(void);
int parseNone();
token* nextToken(void* t);
void setTokenError(char* pos, char code);

#endif
