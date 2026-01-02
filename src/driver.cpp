#include "driver.hpp"
#include <Arduino.h>


#define PIN_RED 2
#define PIN_YELLOW 4
#define PIN_GREEN 5


void initRedLight() {
    pinMode(PIN_RED, OUTPUT);
    digitalWrite(PIN_RED, LOW);
}
void initYellowLight() {
    pinMode(PIN_YELLOW, OUTPUT);
    digitalWrite(PIN_YELLOW, LOW);
}
void initGreenLight() {
    pinMode(PIN_GREEN, OUTPUT);
    digitalWrite(PIN_GREEN, LOW);
}


void turnOnRedLight() {
    digitalWrite(PIN_RED, HIGH);
}
void turnOffRedLight() {
    digitalWrite(PIN_RED, LOW);
}


void turnOnYellowLight() {
    digitalWrite(PIN_YELLOW, HIGH);
}
void turnOffYellowLight() {
    digitalWrite(PIN_YELLOW, LOW);
}


void turnOnGreenLight() {
    digitalWrite(PIN_GREEN, HIGH);
}
void turnOffGreenLight() {
    digitalWrite(PIN_GREEN, LOW);
}