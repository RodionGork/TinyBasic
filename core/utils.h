#ifndef __UTILS_H_
#define __UTILS_H_

#include "mytypes.h"

void trim(char* s);
char* skipSpaces(char* s);
char* skipDigits(char* s);
char charInStr(char c, char* s);
int decFromStr(char* s);
char isDigit(char c);
char isAlpha(char c);
char isAlNum(char c);
char isSpace(char c);
char toUpper(char c);

char input(char* s, short n);
void outputChar(char c);
void outputStr(char* s);
void outputNStr(nstring* s);
void outputInt(long n);
void outputCr();

short sysGetc(void);
void sysPutc(char c);
short adcRead(char channel);
char pinRead(char pin);
void pinOut(char pin, char state);
void sysDelay(short ms);
void sysQuit(void);
char storageOperation(void* data, short size);

#endif

