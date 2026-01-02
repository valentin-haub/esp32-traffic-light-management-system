#include "driver.hpp"
#include <Arduino.h>


#define PIN_ROT 2
#define PIN_GELB 4
#define PIN_GRUEN 5


void initRedLight() {
    pinMode(PIN_ROT, OUTPUT);
    digitalWrite(PIN_ROT, LOW);
}
void initYellowLight() {
    pinMode(PIN_GELB, OUTPUT);
    digitalWrite(PIN_GELB, LOW);
}
void initGreenLight() {
    pinMode(PIN_GRUEN, OUTPUT);
    digitalWrite(PIN_GRUEN, LOW);
}


void turnOnRedLight() {
    digitalWrite(PIN_ROT, HIGH);
}
void turnOffRedLight() {
    digitalWrite(PIN_ROT, LOW);
}


void turnOnYellowLight() {
    digitalWrite(PIN_GELB, HIGH);
}
void turnOffYellowLight() {
    digitalWrite(PIN_GELB, LOW);
}


void turnOnGreenLight() {
    digitalWrite(PIN_GRUEN, HIGH);
}
void turnOffGreenLight() {
    digitalWrite(PIN_GRUEN, LOW);
}