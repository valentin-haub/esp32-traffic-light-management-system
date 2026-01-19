#include <Arduino.h>
#include "TrafficLight.hpp"


// PINs
#define PIN_RED_1 23
#define PIN_YELLOW_1 22
#define PIN_GREEN_1 21

#define PIN_RED_2 16
#define PIN_YELLOW_2 4
#define PIN_GREEN_2 2

#define PIN_REQUEST_1 19
#define PIN_REQUEST_2 15

#define PIN_LDR 12
#define PIN_POTI 13


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
          int greenTime = tl1.vars.greenTime / 1000;
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


    vTaskDelay(pdMS_TO_TICKS(200));
  }
}



void setup() {
  Serial.begin(115200);

  q1 = xQueueCreate(20, sizeof(TrafficLight_EventId));
  q2 = xQueueCreate(20, sizeof(TrafficLight_EventId));

  xTaskCreate(taskClock, "Clock Task", 1024, NULL, 3, &hClock);
  xTaskCreate(taskTrafficLight1, "TrafficLight1 Task", 2048, NULL, 2, &hTrafficLight1);
  xTaskCreate(taskTrafficLight2, "TrafficLight2 Task", 2048, NULL, 2, &hTrafficLight2);

  xTaskCreate(taskRequest1, "Request1 Task", 2048, NULL, 1, &hRequest1);
  xTaskCreate(taskRequest2, "Request2 Task", 2048, NULL, 1, &hRequest2);
  xTaskCreate(taskSerial, "Serial Task", 4096, NULL, 1, &hSerial);
  xTaskCreate(taskSensors, "Sensors Task", 1024, NULL, 1, &hSensors);
}


void loop() {
  vTaskDelete(NULL);
}