#include "EEPROM.h"

#include "main.h"

#include "tokens.h"

#define PROG_SPACE_SIZE 1000
#define VARS_SPACE_SIZE 200

char* extraCmds[] = {
    "POKE",
    "PIN",
    "DELAY",
    "",
};

char extraCmdArgCnt[] = {2, 2, 1};

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
    short c = Serial.read();
    if (c >= ' ') {
      Serial.write(c);
    }
    if (c == '\r') {
      Serial.write('\r');
      Serial.write('\n');
    }
    return c;
}

void sysPutc(char c) {
    Serial.write(c);
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

void extraCommand(char cmd, numeric args[]) {
    switch (cmd) {
        case 0:
            poke(args[0], args[1]);
            break;
        case 1:
            pinOut(args[0], args[1]);
            break;
        case 2:
            delay(args[0]);
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

char storageOperation(void* data, short size) {
    short i;
    if (data == NULL) {
        filePtr = 0;
        return 1;
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
    Serial.begin(115200);
    init(dataSpace, VARS_SPACE_SIZE);
}

void loop() {
    dispatch();
}

