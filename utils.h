#ifndef __UTILS_H_
#define __UTILS_H_

typedef struct nstring {
    unsigned char len;
    char text[1];
} __attribute__((packed)) nstring;

void trim(char* s);
char* skipSpaces(char* s);
char* skipDigits(char* s);
int charInStr(char c, char* s);
int input(char* s, int n);
void outputChar(char c);
void outputStr(char* s);
void outputNStr(nstring* s);
void outputInt(int n);
void outputCr();

#endif

