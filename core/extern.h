#ifndef __EXTERN_H_
#define __EXTERN_H_

extern char* extraCmds[];
extern char extraCmdArgCnt[];
extern char* extraFuncs[];
extern char extraFuncArgCnt[];

short sysGetc(void);
void sysPutc(char c);
void extraCommand(char cmd, numeric args[]);
numeric extraFunction(char cmd, numeric args[]);
void sysQuit(void);
char storageOperation(void* data, short size);

#endif

