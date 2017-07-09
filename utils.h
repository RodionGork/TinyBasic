#ifndef __UTILS_H_
#define __UTILS_H_

#include "mytypes.h"

void trim(char* s);
char* skipSpaces(char* s);
char* skipDigits(char* s);
char charInStr(char c, char* s);
char input(char* s, short n);
void outputChar(char c);
void outputStr(char* s);
void outputNStr(nstring* s);
void outputInt(long n);
void outputCr();

short sysgetc(void);
void sysputc(char c);
short adcread(char channel);
char pinread(char pin);
void pinout(char pin, char state);
void sysdelay(short ms);

#endif

