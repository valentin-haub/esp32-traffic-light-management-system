#include <Arduino.h>
#include "TrafficLight.hpp"
#include "driver.hpp"


// PINs
#define PIN_RED_1 23
#define PIN_YELLOW_1 22
#define PIN_GREEN_1 21

#define PIN_RED_2 18
#define PIN_YELLOW_2 5
#define PIN_GREEN_2 17

#define PIN_REQUEST_1 19
#define PIN_REQUEST_2 16

#define PIN_LDR 12
#define PIN_POTI 13

#define PIN_BI_0 32
#define PIN_BI_1 33
#define PIN_BI_2 25
#define PIN_BI_3 26

#define PIN_SERVO 15

#define PIN_I2C_SDA 27
#define PIN_I2C_SCL 14


TrafficLight tl1;
TrafficLight tl2;

QueueHandle_t q1;
QueueHandle_t q2;

TaskHandle_t hTrafficLight1;
TaskHandle_t hTrafficLight2;
TaskHandle_t hRequest1;
TaskHandle_t hRequest2;
TaskHandle_t hClock;
TaskHandle_t hSerial;
TaskHandle_t hSensors;


void taskTrafficLight1(void* pvParemeters){
  TrafficLight_EventId event;

  TrafficLight_ctor(&tl1);

  // Pins setzen
  tl1.vars.pinRed = PIN_RED_1;
  tl1.vars.pinYellow = PIN_YELLOW_1;
  tl1.vars.pinGreen = PIN_GREEN_1;

  // Startwerte setzen
  tl1.vars.time = 0;
  tl1.vars.greenTime = 5000; // 5 Sekunden
  tl1.vars.brightness = 4095; // Start als "hell"
  tl1.vars.threshold = 2000;
  tl1.vars.barrierActive = false;

  TrafficLight_start(&tl1);
  
  while (1){
    if (xQueueReceive(q1, &event, portMAX_DELAY) == pdPASS){
      TrafficLight_dispatch_event(&tl1, event);
    }
  }
}

void taskTrafficLight2(void* pvParemeters){
  TrafficLight_EventId event;

  TrafficLight_ctor(&tl2);

  // Pins setzen
  tl2.vars.pinRed = PIN_RED_2;
  tl2.vars.pinYellow = PIN_YELLOW_2;
  tl2.vars.pinGreen = PIN_GREEN_2;

  // Startwerte setzen
  tl2.vars.time = 0;
  tl2.vars.greenTime = 5000; // 5 Sekunden
  tl2.vars.brightness = 4095; // Start als "hell"
  tl2.vars.threshold = 2000;
  tl2.vars.barrierActive = false;

  TrafficLight_start(&tl2);
  
  while (1){
    if (xQueueReceive(q2, &event, portMAX_DELAY) == pdPASS){
      TrafficLight_dispatch_event(&tl2, event);
    }
  }
}

void taskRequest1(void* pvParameters){
  int lastRequestState = HIGH;
  int currentRequestState;

  pinMode(PIN_REQUEST_1, INPUT_PULLUP);

  while(1){
    currentRequestState = digitalRead(PIN_REQUEST_1);

    if (lastRequestState == HIGH && currentRequestState == LOW) {
      TrafficLight_EventId request = TrafficLight_EventId_REQUESTGREEN;
      xQueueSend(q1, &request, 0);
    }
    lastRequestState = currentRequestState;

    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void taskRequest2(void* pvParameters){
  int lastRequestState = HIGH;
  int currentRequestState;

  pinMode(PIN_REQUEST_2, INPUT_PULLUP);

  while(1){
    currentRequestState = digitalRead(PIN_REQUEST_2);

    if (lastRequestState == HIGH && currentRequestState == LOW) {
      TrafficLight_EventId request = TrafficLight_EventId_REQUESTGREEN;
      xQueueSend(q2, &request, 0);
    }
    lastRequestState = currentRequestState;

    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void taskClock(void* pvParameters){
  TrafficLight_EventId tick = TrafficLight_EventId_TICK;

  while(1){
    xQueueSend(q1, &tick, 0);
    xQueueSend(q2, &tick, 0);

    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void taskSerial(void* pvParameters){
  char input;
  const char* menu = "Wählen Sie einen Eintrag per Eingabe der Nummer:\r\n"
                      "1. Auslesen des Leuchzustände der Ampeln\r\n"
                      "2. Setzen des Requests bei einer Ampel\r\n"
                      "3. Auslesen der aktuellen Dauer der Grünphase\r\n"
                      "4. Auslesen der aktuellen Helligkeit\r\n\r\n"
                      "Auswahl: ";
  
  const char* tlChoose = "Bitte Ampel per Nummer wählen:\r\n"
                      "1. Ampel 1\r\n"
                      "2. Ampel 2\r\n\r\n"
                      "Auswahl: ";

  Serial.print(menu);

  while(1){
    if (Serial.available() > 0){
      input = Serial.read();

      if (input == '\n' || input == '\r' || input == ' ') continue;

      if (input == '1' || input == '2' || input == '3' || input == '4'){
        Serial.print(input);
        Serial.println();

        if (input == '1'){
          const char* currentState1 = TrafficLight_state_id_to_string(tl1.state_id);
          const char* currentState2 = TrafficLight_state_id_to_string(tl2.state_id);

          if (tl1.state_id == TrafficLight_StateId_BLINKON || tl1.state_id == TrafficLight_StateId_BLINKOFF){
            currentState1 = "NIGHTMODE";
          }
          if (tl2.state_id == TrafficLight_StateId_BLINKON || tl2.state_id == TrafficLight_StateId_BLINKOFF){
            currentState2 = "NIGHTMODE";
          }

          Serial.print("Der aktuelle Zustand der Ampel ist: ");
          Serial.print(currentState1);
          Serial.print("\r\n");
          Serial.print("Der aktuelle Zustand der Ampel ist: ");
          Serial.print(currentState2);
        }
        else if (input == '2'){
          TrafficLight_EventId request = TrafficLight_EventId_REQUESTGREEN;

          Serial.print(tlChoose);

          while (Serial.available() == 0){
            vTaskDelay(pdMS_TO_TICKS(10));
          }
          input = Serial.read();
          if (input == '\n' || input == '\r' || input == ' ') continue;
          if (input == '1' || input == '2'){
            Serial.print(input);
            Serial.println();

            if (input == '1'){
              xQueueSend(q1, &request, 0);
              Serial.print("Request für Ampel 1 wurde gesetzt.");
            }
            else if (input == '2'){
              xQueueSend(q2, &request, 0);
              Serial.print("Request für Ampel 2 wurde gesetzt.");
            }
          }
        }
        else if (input == '3'){
          float greenTime = tl1.vars.greenTime / 1000.0;
          Serial.print("Die Dauer der Grünphasen beträgt: ");
          Serial.print(greenTime);
          Serial.print(" Sek.");
        }
        else if (input == '4'){
          int brightness = tl1.vars.brightness;
          Serial.print("Die aktuelle Helligkeit beträgt: ");
          Serial.print(brightness);
        }

        Serial.println();
        Serial.println();
        Serial.print(menu);
      }
      else {
        Serial.println();
        Serial.println("\r\nUngueltige Eingabe. Bitte '1', '2', '3' oder '4' eingeben.\r\n\r\n");
        Serial.print(menu);
      }
    }
    else {
      vTaskDelay(pdMS_TO_TICKS(20));
    }
  }
}

void taskSensors(void* pvParameters){
  pinMode(PIN_LDR, INPUT);
  pinMode(PIN_POTI, INPUT);

  initBinaryDisplay(PIN_BI_0, PIN_BI_1, PIN_BI_2, PIN_BI_3);
  initBarrierSystem(PIN_SERVO, PIN_I2C_SDA, PIN_I2C_SCL);
  initDisplay();


  unsigned long barrierClosedTimestamp = 0;
  bool wasBarrierOpen = true;

  while(1){
    // Brightness aktualisieren
    int ldrValue = analogRead(PIN_LDR);
    int brightness = map(ldrValue, 0, 4095, 4095, 0);
    tl1.vars.brightness = brightness;
    tl2.vars.brightness = brightness;

    // Grünphasenzeit aktualisieren
    int potiValue = analogRead(PIN_POTI);
    int greenTime = map(potiValue, 0, 4095, 1000, 10000); // 1 - 10 Sekunden
    tl1.vars.greenTime = greenTime;
    tl2.vars.greenTime = greenTime;

    // Grünphase Status-LEDs aktualisieren
    int statusValue = map(greenTime, 1000, 10000, 0, 15);
    refreshBinaryStatus(statusValue, PIN_BI_0, PIN_BI_1, PIN_BI_2, PIN_BI_3);


    // Schranken-Steuerung

    // Auslesen der Neigung des Hebels und Berechnen der Neigung
    float inclination = getBarrierInclination(); // Neigung Y-Achse

    int targetAngle = map((int)(inclination * 10), 0, 98, 0, 90); // 0 - 9.8 --> 0 - 90
    targetAngle = constrain(targetAngle, 0, 90); // Begrenzung
    
    // Einstellen des Servo-Motors
    bool isAmpelRed = (tl1.state_id == TrafficLight_StateId_TRAFFICLIGHTRED);
    if (isAmpelRed) {
      setBarrierServo(targetAngle);
    } else {
      setBarrierServo(0);
      targetAngle = 0;
    }

    bool isBarrierClosed = (targetAngle < 5);
    long remainingTime = 0;

    if (!isBarrierClosed) {
      wasBarrierOpen = true;
      tl1.vars.barrierActive = false;
      tl2.vars.barrierActive = false;
      remainingTime = 0;
    } 
    else {
      if (wasBarrierOpen) {
        barrierClosedTimestamp = millis(); // Stoppuhr starten
        wasBarrierOpen = false;
      }

      unsigned long elapsed = millis() - barrierClosedTimestamp;

      // Check für den 4 Sekunden Guard
      if (elapsed > 4000) {
        tl1.vars.barrierActive = true;
        tl2.vars.barrierActive = true;
        remainingTime = 0;
      } else {
        tl1.vars.barrierActive = false;
        tl2.vars.barrierActive = false;
        remainingTime = 4000 - elapsed;
      }
    }

    // Anzeigen auf dem Display
    showStatus(inclination, targetAngle, isBarrierClosed, remainingTime);


    vTaskDelay(pdMS_TO_TICKS(100));
  }
}



void setup() {
  Serial.begin(115200);

  q1 = xQueueCreate(20, sizeof(TrafficLight_EventId));
  q2 = xQueueCreate(20, sizeof(TrafficLight_EventId));

  xTaskCreate(taskClock, "Clock Task", 1024, NULL, 3, &hClock);
  xTaskCreate(taskTrafficLight1, "TrafficLight1 Task", 2048, NULL, 2, &hTrafficLight1);
  xTaskCreate(taskTrafficLight2, "TrafficLight2 Task", 2048, NULL, 2, &hTrafficLight2);

  xTaskCreate(taskSensors, "Sensors Task", 2048, NULL, 2, &hSensors);
  xTaskCreate(taskRequest1, "Request1 Task", 2048, NULL, 1, &hRequest1);
  xTaskCreate(taskRequest2, "Request2 Task", 2048, NULL, 1, &hRequest2);
  xTaskCreate(taskSerial, "Serial Task", 4096, NULL, 1, &hSerial);
}


void loop() {
  vTaskDelete(NULL);
}