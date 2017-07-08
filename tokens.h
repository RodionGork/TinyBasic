#ifndef __TOKENS_H_
#define __TOKENS_H_

#define TT_NONE 0x00
#define TT_ERROR 0x01
#define TT_NUMBER 0x10
#define TT_NAME 0x20
#define TT_COMMAND 0x21
#define TT_VARIABLE 0x22
#define TT_FUNCTION 0x23
#define TT_SYMBOL 0x30
#define TT_FUNC_END 0x31
#define TT_SEPARATOR 0x32
#define TT_LITERAL 0x40
#define TT_COMMENT 0x41

#define CMD_REM 0
#define CMD_PRINT 1
#define CMD_INPUT 2
#define CMD_IF 3
#define CMD_GOTO 4
#define CMD_GOSUB 5
#define CMD_RETURN 6
#define CMD_END 7
#define CMD_PIN 8

#define MAX_LINE_NUMBER 30000

#include "utils.h"

typedef union tbody {
    short integer;
    char symbol;
    char command;
    nstring str;
} tbody;

typedef struct token {
    char type;
    tbody body;
} __attribute__((packed)) token;

void parseLine(char* line, void* tokens);
int tokenSize(token* t);
token* nextToken(void* t);
void copyToken(void* dst, void* src);
int tokenClass(token* t);
int tokenNameEqual(token* t, char* s);
char* getParseErrorPos(void);
char* getParseErrorMsg(void);

#endif

