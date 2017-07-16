#include "stm32f103.h"

#define UART_RX_BUF_SIZE 8

#define uchar unsigned char

char hex[] = "0123456789ABCDEF";
unsigned char uartRxBuf[UART_RX_BUF_SIZE];
char uartRxBufStart, uartRxBufEnd;

void enableInterrupts(void) {
    __asm("cpsie i");
}

void irqEnable(unsigned char n) {
    REG_L(NVIC_BASE, NVIC_ISER + (n / 32) * 4) |= (1 << (n % 32));
}

void pinMode(int base, char num, char mode, char cnf) {
    int* p = (int*) (void*) (base + (num < 8 ? GPIO_CRL : GPIO_CRH));
    num &= 0x7;
    char offs = num * 4;
    int v = *p;
    v &= ~(0xF << offs);
    v |= (mode | (cnf << 2)) << offs;
    *p = v;
}

void pinOutput(int base, char num, char v) {
    if (v == 0) {
        num += 16;
    }
    REG_L(base, GPIO_BSRR) |= 1 << num;
}

void uartEnable(int divisor) {
    REG_L(RCC_BASE, RCC_APB2ENR) |= (1 << 0); // AFIO clock
    pinMode(GPIOA_BASE, 9, PIN_MODE_OUT, PIN_CNF_O_APP);
    REG_L(AFIO_BASE, AFIO_MAPR) &= ~(1 << 2); // no UART1 remap
    REG_L(RCC_BASE, RCC_APB2ENR) |= (1 << 14); // UART clock
    REG_L(USART_BASE, USART_BRR) |= divisor;
    REG_L(USART_BASE, USART_CR1) |= (1 << 13); // UART enable
    REG_L(USART_BASE, USART_CR1) |= (3 << 2); // UART transmit/receive enable
    REG_L(USART_BASE, USART_CR1) |= (1 << 5); // UART receive interrupt enable
    uartRxBufStart = 0;
    uartRxBufEnd = 0;
    irqEnable(IRQ_UART);
}

int uartRead(void) {
    unsigned char c;
    if (uartRxBufStart == uartRxBufEnd) {
        return -1;
    }
    c = uartRxBuf[uartRxBufStart];
    uartRxBufStart = (uartRxBufStart + 1) % UART_RX_BUF_SIZE;
    return c;
}

void uartIrqHandler(void) {
    char next = (uartRxBufEnd + 1) % UART_RX_BUF_SIZE;
    uartRxBuf[uartRxBufEnd] = REG_B(USART_BASE, USART_DR);
    if (next != uartRxBufStart) {
        uartRxBufEnd = next;
    }
}

void uartSend(int c) {
    while ((REG_L(USART_BASE, USART_SR) & (1 << 7)) == 0);
    REG_L(USART_BASE, USART_DR) = c;
}

void uartSends(char* s) {
    while (*s) {
        uartSend(*(s++));
    }
}

void uartSendHex(int x, int d) {
    while (d-- > 0) {
        uartSend(hex[(x >> (d * 4)) & 0xF]);
    }
}

void uartSendDec(int x) {
    static char s[10];
    int i, x1;
    i = 0;
    while (x > 0) {
        x1 = x / 10;
        s[i++] = x - x1 * 10;
        x = x1;
    }
    if (i == 0) {
        s[i++] = 0;
    }
    while (i > 0) {
        uartSend('0' + s[--i]);
    }
}

void setupPll(int mhz) {
    int boost = mhz / 4 - 2;
    REG_L(RCC_BASE, RCC_CR) &= ~(1 << 24); // PLLON = 0
    while ((REG_L(RCC_BASE, RCC_CR) | (1 << 25)) == 0);
    REG_L(RCC_BASE, RCC_CFGR) = (boost & 0xF) << 18;
    REG_L(RCC_BASE, RCC_CR) |= (1 << 24); // PLLON = 1
    while ((REG_L(RCC_BASE, RCC_CR) | (1 << 25)) == 0);
    REG_L(RCC_BASE, RCC_CFGR) |= (1 << 1);
    while (((REG_L(RCC_BASE, RCC_CFGR) >> 2) & 0x3) != 2);
}

int strlen(const char* s) {
    int i;
    for (i = 0; s[i]; i++) {
    }
    return i;
}

void* memcpy(void* dst, const void* src, int sz) {
    char* d = (char*) dst;
    char* s = (char*) src;
    while (sz-- > 0) {
        *(d++) = *(s++);
    }
    return dst;
}

int memcmp(const void* dst, const void* src, int sz) {
    uchar* d = (uchar*) dst;
    uchar* s = (uchar*) src;
    int i, v;
    for (i = 0; i < sz; i++) {
        v = *(d++) - *(s++);
        if (v != 0) {
            return v;
        }
    }
    return 0;
}

void* memmove(void* dst, const void* src, int sz) {
    uchar* d = (uchar*) dst;
    uchar* s = (uchar*) src;
    int i;
    if (d < s) {
        for (i = 0; i < sz; i++) {
            *(d++) = *(s++);
        }
    } else {
        d += sz;
        s += sz;
        for (i = 0; i < sz; i++) {
            *(--d) = *(--s);
        }
    }
    return dst;
}

void sysPutc(char c) {
    uartSend(c);
}

short sysGetc(void) {
    short c = uartRead();
    if (c >= ' ') {
        uartSend(c);
    }
    if (c == '\r') {
        uartSend('\r');
        uartSend('\n');
    }
    return c;
}

short adcRead(char channel) {
    return 0;
}

short pinRead(char pin) {
    return 0;
}

void pinOut(char pin, char state) {
}

void sysDelay(short ms) {
}

void sysQuit(void) {
}

char storageOperation(void* data, short size) {
    return 0;
}
