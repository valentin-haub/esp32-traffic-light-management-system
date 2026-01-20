#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


void initLight(uint8_t pin);

void turnOnLight(uint8_t pin);

void turnOffLight(uint8_t pin);


void initBinaryDisplay(uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3);

// Aktualisiert die Status-LEDs
void refreshBinaryDisplay(uint8_t value, uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3);


void initBarrierSystem(uint8_t servoPin, uint8_t sdaPin, uint8_t sclPin);

// Neigung der Y-Achse
float getBarrierInclination();

// Winkel im Servo-Motor einstellen
void setBarrierServo(int angle);

// Display
void initDisplay();
void showStatus(float inclination, bool barrierIsSafe);