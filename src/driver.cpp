#include "driver.hpp"
#include <Arduino.h>


void initLight(uint8_t pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void turnOnLight(uint8_t pin) {
    digitalWrite(pin, HIGH);
}

void turnOffLight(uint8_t pin) {
    digitalWrite(pin, LOW);
}
