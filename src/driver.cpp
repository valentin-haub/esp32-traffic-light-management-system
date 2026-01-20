#include "driver.hpp"


Adafruit_MPU6050 mpu;
Servo barrierServo;


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


void initBarrierSystem(uint8_t servoPin, uint8_t sdaPin, uint8_t sclPin) {
    Wire.begin(sdaPin, sclPin); // I2C Bus starten

    // MPU6050 starten
    if (!mpu.begin()) {
        Serial.println("Fehler: MPU6050 nicht gefunden!");
    }
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    // Servo starten (50Hz, 0.5ms - 2.5ms)
    barrierServo.setPeriodHertz(50);
    barrierServo.attach(servoPin, 500, 2500);
}

float getBarrierInclination() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    return a.acceleration.y; // Neigung Y-Achse
}

void setBarrierServo(int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    
    barrierServo.write(angle);
}