#ifndef __TOKENS_H_
#define __TOKENS_H_

#define TT_NONE 0x00
#define TT_ERROR 0x01
#define TT_NUMBER 0x10
#define TT_NAME 0x20
#define TT_SYMBOL 0x30
#define TT_LITERAL 0x40

#include "utils.h"

typedef union tbody {
    short integer;
    char symbol;
    nstring str;
} tbody;

typedef struct token {
    char type;
    tbody body;
} __attribute__((packed)) token;

void parseLine(char* line, void* tokens);
int tokenSize(token* t);
int tokenClass(token* t);
int tokenNameEqual(token* t, char* s);
char* getParseError(void);

#endif

