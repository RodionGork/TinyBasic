#include "EEPROM.h"

#include "main.h"

#include "tokens.h"

short filePtr;

short sysGetc(void) {
    return Serial.read();
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

void sysDelay(short ms) {
    delay(ms);
}

void sysQuit(void) {
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

char dataSpace[1200];

void setup() {
    Serial.begin(115200);
    init(dataSpace, 200);
}

void loop() {
    dispatch();
}

