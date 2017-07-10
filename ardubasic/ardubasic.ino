#include "main.h"

#include "tokens.h"

short sysgetc(void) {
    return Serial.read();
}

void sysputc(char c) {
    Serial.write(c);
}

short adcread(char channel) {
    return analogRead(channel);
}

char pinread(char pin) {
    return (digitalRead(pin) == HIGH) ? 1 : 0;
}

void pinout(char pin, char state) {
    if (state >= 0) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, state ? HIGH : LOW);
    } else {
        pinMode(pin, (state == -1) ? INPUT : INPUT_PULLUP);
    }
}

void sysdelay(short ms) {
    delay(ms);
}

void syssave(char id, char* data) {
}

void sysload(char id, char* data) {
}

void sysquit(void) {
}

char dataSpace[1200];

void setup() {
    Serial.begin(115200);
    init(dataSpace, 200);
}

void loop() {
    dispatch();
}

