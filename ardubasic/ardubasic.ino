#include "EEPROM.h"

#include "main.h"

#include "tokens.h"

#define UART_SPEED 115200

#define PROG_SPACE_SIZE 500
#define VARS_SPACE_SIZE 150

char* extraCmds[] = {
    "POKE",
    "PIN",
    "",
};

char extraCmdArgCnt[] = {2, 2};

char* extraFuncs[] = {
    "PEEK",
    "PIN",
    "ADC",
    "",
};

char dataSpace[VARS_SPACE_SIZE + PROG_SPACE_SIZE];

char extraFuncArgCnt[] = {1, 1, 1};

short filePtr;

short sysGetc(void) {
    return Serial.read();
}

void sysPutc(char c) {
    if (c == '\n') {
      Serial.write('\r');
    } else if (c == '\b') {
        Serial.write('\b');
        Serial.write(' ');
    }
    Serial.write(c);
}

void sysEcho(char c) {
    sysPutc(c);
}

short adcRead(char channel) {
    return analogRead(channel);
}

char pinRead(char pin) {
    return (digitalRead(pin) == HIGH) ? 1 : 0;
}

void pinOut(char pin, char state) {
    if (state >= 0) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, state ? HIGH : LOW);
    } else {
        pinMode(pin, (state == -1) ? INPUT : INPUT_PULLUP);
    }
}

void poke(short addr, uchar value) {
    *((uchar*) addr) = value;
}

uchar peek(short addr) {
    return *((uchar*) addr);
}

void sysQuit(void) {
}

void sysDelay(numeric pause) {
    delay(pause);
}

void extraCommand(char cmd, numeric args[]) {
    switch (cmd) {
        case 0:
            poke(args[0], args[1]);
            break;
        case 1:
            pinOut(args[0], args[1]);
            break;
    }
}

numeric extraFunction(char cmd, numeric args[]) {
    switch (cmd) {
        case 0:
            return peek(args[0]);
        case 1:
            return pinRead(args[0]);
        case 2:
            return adcRead(args[0]);
    }
    return 0;
}

unsigned char storageChecksum(short size) {
    unsigned char res = 0;
    while (size > 0) {
        res ^= EEPROM.read(--size);
    }
    return res;
}

char storageOperation(void* data, short size) {
    short i;
    if (data == NULL) {
        if (size == 0) {
            EEPROM.write(filePtr, storageChecksum(filePtr));
            return 1;
        } else {
            filePtr = 0;
            if (size > 0) {
                return 1;
            }
            size = EEPROM.read(1);
            size = ((size << 8) | EEPROM.read(0)) + 2;
            if (size <= 0 || size >= PROG_SPACE_SIZE) {
                return 0;
            }
            return storageChecksum(size + 1) == 0;
        }
    }
    if (size > 0) {
        for (i = 0; i < size; i += 1) {
            EEPROM.write(filePtr++, ((unsigned char*)data)[i]);
        }
    } else {
        size = -size;
        for (i = 0; i < size; i += 1) {
            ((unsigned char*)data)[i] = EEPROM.read(filePtr++);
        }
    }
    return 1;
}

void setup() {
    Serial.begin(UART_SPEED);
    init(dataSpace, VARS_SPACE_SIZE);
}

void loop() {
    dispatch();
}

