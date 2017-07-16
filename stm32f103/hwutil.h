#ifndef __HWUTIL_H_
#define __HWUTIL_H_

extern const void * Vectors[];

void pinMode(int base, char num, char mode, char cnf);
void pinOutput(int base, char num, char v);

void uartEnable(int divisor);
int uartRead(void);
void uartSend(int c);
void uartSends(char* s);
void uartSendHex(int x, int d);
void uartSendDec(int x);
void setupPll(int mhz);
void enableInterrupts(void);

#endif

