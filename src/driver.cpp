#include "driver.hpp"


Adafruit_MPU6050 mpu;
Servo barrierServo;
Adafruit_SSD1306 display(128, 64, &Wire, -1);


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

void refreshBinaryStatus(uint8_t value, uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3){
    digitalWrite(p0, (value & 0x01) ? HIGH : LOW);
    digitalWrite(p1, (value & 0x02) ? HIGH : LOW);
    digitalWrite(p2, (value & 0x04) ? HIGH : LOW);
    digitalWrite(p3, (value & 0x08) ? HIGH : LOW);
}


void initBarrierSystem(uint8_t servoPin, uint8_t sdaPin, uint8_t sclPin){
    Wire.begin(sdaPin, sclPin); // I2C Bus starten
    Wire.setClock(400000); // 400 kHz

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

// Neigung der Y-Achse
float getBarrierInclination(){
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    return a.acceleration.y;
};

// Winkel im Servo-Motor einstellen
void setBarrierServo(int angle){
    if (angle < 0) angle = 0;
    if (angle > 90) angle = 90;
    barrierServo.write(angle);
};


void initDisplay(){
    // Adresse 0x3C für 128x64 OLED
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 nicht gefunden"));
    }
    display.clearDisplay();
    display.display();
}

void showStatus(float inclination, int angle, bool barrierClosed, long remainingTime){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 0);

    display.println(">> SCHRANKE <<");
    display.println("-------------------");
    display.print("Neigung: ");
    display.print(inclination, 1); // 1 Nachkommastelle

    display.println(" m/s^2");
    display.print("Winkel: ");
    display.print(angle);
    display.println((char)247);
    
    display.println("");
    display.print("Status: ");
    
    // Status anzeigen
    if (!barrierClosed) {
        display.println("OFFEN");
    } else {
        if (remainingTime <= 0){
            display.println("GESCHLOSSEN");
        } else {
            display.print("WARTEN: ");
            display.print(remainingTime / 1000.0, 1); // 1 Nachkommastelle
            display.println(" s");
        }
    }

    display.display();
}