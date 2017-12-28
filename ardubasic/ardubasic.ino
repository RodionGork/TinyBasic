#include <EEPROM.h>
#include <avr/pgmspace.h>

#include "main.h"
#include "mytypes.h"
#include "textual.h"
#include "tokens.h"

#define UART_SPEED 115200

#define PROG_SPACE_SIZE 700
#define VARS_SPACE_SIZE 300

char extraCmdArgCnt[] = {2, 2};

char extraFuncArgCnt[] = {1, 1, 1};

static const char commonStrings[] PROGMEM = CONST_COMMON_STRINGS;
static const char parsingErrors[] PROGMEM = CONST_PARSING_ERRORS;

char dataSpace[VARS_SPACE_SIZE + PROG_SPACE_SIZE];

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
    if (channel == -1) {
        analogRead(0);
        ADMUX = (ADMUX & 0xF0) | 0x0E;
        delay(1);
        ADCSRA |= 0x40;
        while (ADCSRA & 0x40);
        short v = ADC;
        return v > 0 ? (1100L * 1023) / v : -1;
    }
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

void outputConstStr(char strId, char index, char* w) {
    const char* s;
    int k = 0;
    char c;
    switch (strId) {
        case ID_COMMON_STRINGS:
            s = commonStrings;
            break;
        case ID_PARSING_ERRORS:
            s = parsingErrors;
            break;
        default:
            return;
    }
    while (index > 0) {
        do {
            c = pgm_read_byte_near(s + k);
            k += 1;
        } while (c != '\n');
        index -= 1;
    }
    while (1) {
        c = pgm_read_byte_near(s + k);
        if (c == '\n') {
            break;
        }
        if (w != NULL) {
            *(w++) = c;
        } else {
            sysPutc(c);
        }
        k += 1;
    }
    if (w != NULL) {
        *w = 0;
    }
}

short extraCommandByHash(numeric h) {
    switch (h) {
        case 0x036F: // POKE
            return CMD_EXTRA + 0;
        case 0x019C: // PIN
            return CMD_EXTRA + 1;
        default:
            return -1;
    }
}

short extraFunctionByHash(numeric h) {
    switch (h) {
        case 0x0355: // PEEK
            return 0;
        case 0x019C: // PIN
            return 1;
        case 0x01CF: // ADC
            return 2;
        default:
            return -1;
    }
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

