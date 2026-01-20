#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>


void initLight(uint8_t pin);

void turnOnLight(uint8_t pin);

void turnOffLight(uint8_t pin);


void initBinaryDisplay(uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3);

// Aktualisiert die Status-LEDs
void refreshBinaryDisplay(uint8_t value, uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3);


void initBarrierSystem(uint8_t servoPin, uint8_t sdaPin, uint8_t sclPin);

// Gibt die Neigung (Y-Achse) zurück
float getBarrierInclination(); 

// Setzt den Servo auf einen Winkel (0-180)
void setBarrierServo(int angle);