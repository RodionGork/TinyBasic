#ifndef __EXTERN_H_
#define __EXTERN_H_

extern char extraCmdArgCnt[];
extern char extraFuncArgCnt[];

short extraCommandByHash(numeric h);
short extraFunctionByHash(numeric h);

short sysGetc(void);
void sysPutc(char c);
void sysEcho(char c);
void sysDelay(numeric t);
void extraCommand(char cmd, numeric args[]);
numeric extraFunction(char cmd, numeric args[]);
void sysQuit(void);
void outputConstStr(char strId, char index, char* s);

/* data=NULL for open/close, size > 0 for write,
size < 0 for read, size=0 for close,
abs(size) can be fileId */
char storageOperation(void* data, short size);

#endif

