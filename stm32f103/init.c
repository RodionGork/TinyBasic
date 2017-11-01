#include "stm32f103.h"
#include "hwutil.h"
#include "../core/main.h"

#define STACK_TOP 0x20005000
#define CLOCK_SPEED 48
#define UART_SPEED 9600

#define PROG_SPACE_SIZE 2048
#define VARS_SPACE_SIZE 512

extern unsigned char  INIT_DATA_VALUES;
extern unsigned char  INIT_DATA_START;
extern unsigned char  INIT_DATA_END;
extern unsigned char  BSS_START;
extern unsigned char  BSS_END;
extern unsigned char  BSS_END;

char dataSpace[VARS_SPACE_SIZE + PROG_SPACE_SIZE];

int main(void);
void resetIrqHandler(void);
void uartIrqHandler(void);

const void * intVectors[76] __attribute__((section(".vectors"))) = {
    (void*) STACK_TOP,
    resetIrqHandler,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0, uartIrqHandler,
};

void memoryInit(void) {
    unsigned char volatile *src;
    unsigned char volatile *dest;
    unsigned len;
    src= &INIT_DATA_VALUES;
    dest= &INIT_DATA_START;
    len= &INIT_DATA_END-&INIT_DATA_START;
    while (len--)
        *dest++ = *src++;
    dest = &BSS_START;
    len = &BSS_END - &BSS_START;
    while (len--)
        *dest++=0;
}

void resetIrqHandler(void) {
    memoryInit();
    main();
}

int main(void) {
    setupPll(CLOCK_SPEED);
    REG_L(RCC_BASE, RCC_APB2ENR) |= (1 << 2) | (1 << 3); // ports A, B
    
    REG_L(FLASH_BASE, FLASH_KEYR) = 0x45670123;
    REG_L(FLASH_BASE, FLASH_KEYR) = 0xCDEF89AB;
    
    uartEnable(CLOCK_SPEED * 1000000 / UART_SPEED);
    enableInterrupts();
    
    init(dataSpace, VARS_SPACE_SIZE);
    dispatch();
}

