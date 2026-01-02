#include <Arduino.h>
#include "TrafficLight.hpp"


#define PIN_TASTER_REQUEST 13

TrafficLight tl;


unsigned long lastTickTime = 0;
int lastRequestState = HIGH;


void setup() {
    Serial.begin(115200);
    
    pinMode(PIN_TASTER_REQUEST, INPUT_PULLUP);

    TrafficLight_ctor(&tl); 
    TrafficLight_start(&tl);
}


void loop() {
    
    // Taster "requestGreen" prüfen
    int currentRequestState = digitalRead(PIN_TASTER_REQUEST);
    if (lastRequestState == HIGH && currentRequestState == LOW) {
        TrafficLight_dispatch_event(&tl, TrafficLight_EventId_REQUESTGREEN);
    }
    lastRequestState = currentRequestState;

    // Zeit-Event "TICK" senden (1 Tick pro Millisekunde)
    if (millis() - lastTickTime >= 1) {
        lastTickTime = millis();
        TrafficLight_dispatch_event(&tl, TrafficLight_EventId_TICK); 
    }
}