#ifndef __MYTYPES_H_
#define __MYTYPES_H_

#define MAX_LINE_LEN 80

//16 bit for arduino, 32 on stm32 and linux
typedef signed int numeric;

typedef signed char schar;

typedef unsigned char uchar;

typedef struct nstring {
    unsigned char len;
    char text[1];
} __attribute__((packed)) nstring;

typedef struct prgline {
    short num;
    nstring str;
} __attribute__((packed)) prgline;

typedef union tbody {
    numeric integer;
    char symbol;
    char command;
    nstring str;
} tbody;

typedef struct token {
    char type;
    tbody body;
} __attribute__((packed)) token;

typedef struct varHolder {
    short name;
    numeric value;
} __attribute__((packed)) varHolder;

typedef struct labelCacheElem {
    short num;
    short offset;
} __attribute__((packed)) labelCacheElem;

#endif

