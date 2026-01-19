#pragma once
#include <Arduino.h>


void initLight(uint8_t pin);

void turnOnLight(uint8_t pin);

void turnOffLight(uint8_t pin);


void initBinaryDisplay(uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3);
void refreshBinaryDisplay(uint8_t value, uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3);