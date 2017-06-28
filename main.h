#ifndef __MAIN_H_
#define __MAIN_H_

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
} token;

#endif

