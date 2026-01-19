#include "driver.hpp"


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


void initBinaryDisplay(uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3){
    pinMode(p0, OUTPUT);
    digitalWrite(p0, LOW);

    pinMode(p1, OUTPUT);
    digitalWrite(p1, LOW);

    pinMode(p2, OUTPUT);
    digitalWrite(p2, LOW);

    pinMode(p3, OUTPUT);
    digitalWrite(p3, LOW);
}

  void refreshBinaryDisplay(uint8_t value, uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3){
    digitalWrite(p0, (value & 0x01) ? HIGH : LOW);
    digitalWrite(p1, (value & 0x02) ? HIGH : LOW);
    digitalWrite(p2, (value & 0x04) ? HIGH : LOW);
    digitalWrite(p3, (value & 0x08) ? HIGH : LOW);
  }