#ifndef __EXTERN_H_
#define __EXTERN_H_

extern char* extraCmds[];
extern char extraCmdArgCnt[];
extern char* extraFuncs[];
extern char extraFuncArgCnt[];

short sysGetc(void);
void sysPutc(char c);
void sysEcho(char c);
void sysDelay(numeric t);
void extraCommand(char cmd, numeric args[]);
numeric extraFunction(char cmd, numeric args[]);
void sysQuit(void);

/* data=NULL for open/close, size > 0 for write,
size < 0 for read, size=0 for close,
abs(size) can be fileId */
char storageOperation(void* data, short size);

#endif

