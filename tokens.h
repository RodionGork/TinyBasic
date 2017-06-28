#ifndef __TOKENS_H_
#define __TOKENS_H_

#define TT_NONE 0x00
#define TT_ERROR 0x01
#define TT_NUMBER 0x10
#define TT_NAME 0x20
#define TT_SYMBOL 0x30
#define TT_LITERAL 0x40

typedef union tbody {
    short integer;
    char symbol;
    char text[1];
} tbody;

typedef struct token {
    char type;
    tbody body;
} __attribute__((packed)) token;

void parseLine(char* line, void* tokens);
int tokenSize(token* t);
int tokenClass(token* t);
char* getParseError(void);

#endif

