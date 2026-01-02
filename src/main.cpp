#include <Arduino.h>
#include "TrafficLight.hpp"


#define PIN_REQUEST 13

TrafficLight tl;

QueueHandle_t q;
TaskHandle_t hTrafficLight;
TaskHandle_t hRequest;
TaskHandle_t hClock;
TaskHandle_t hSerial;


void taskTrafficLight(void* pvParemeters){
  TrafficLight_EventId event;

  TrafficLight_ctor(&tl);
  TrafficLight_start(&tl);
  
  while (1){
    if (xQueueReceive(q, &event, portMAX_DELAY) == pdPASS){
      TrafficLight_dispatch_event(&tl, event);
    }
  }
}

void taskRequest(void* pvParameters){
  int lastRequestState = HIGH;
  int currentRequestState;

  pinMode(PIN_REQUEST, INPUT_PULLUP);

  while(1){
    currentRequestState = digitalRead(PIN_REQUEST);

    if (lastRequestState == HIGH && currentRequestState == LOW) {
      TrafficLight_EventId request = TrafficLight_EventId_REQUESTGREEN;
      xQueueSend(q, &request, 0);
    }
    lastRequestState = currentRequestState;

    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void taskClock(void* pvParameters){
  TrafficLight_EventId tick = TrafficLight_EventId_TICK;

  while(1){
    xQueueSend(q, &tick, 0);

    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void taskSerial(void* pvParameters){
  char input;
  const char* menu = "Wählen Sie einen Eintrag per Eingabe der Nummer:\n"
                      "1. Auslesen des Leuchzustands einer Ampel\n"
                      "2. Setzen des Requests bei einer Ampel\n\n"
                      "Auswahl: ";

  Serial.print(menu);

  while(1){
    if (Serial.available() > 0){
      input = Serial.read();

      if (input == '1' || input == '2'){
        Serial.print(input);
        Serial.println();

        if (input == '1'){
          const char* currentState = TrafficLight_state_id_to_string(tl.state_id);
          Serial.print("Der aktuelle Zustand der Ampel ist: ");
          Serial.print(currentState);
        }
        else if (input == '2'){
          TrafficLight_EventId request = TrafficLight_EventId_REQUESTGREEN;
          xQueueSend(q, &request, 0);
          Serial.print("Request wurde gesetzt.");
        }

        Serial.println();
        Serial.println();
        Serial.print(menu);
      }
      else {
        Serial.println();
        Serial.println("\nUngueltige Eingabe. Bitte '1' oder '2' eingeben.\n\n");
        Serial.print(menu);
      }
    }
    else {
      vTaskDelay(pdMS_TO_TICKS(20));
    }
  }
}



void setup() {
  Serial.begin(115200);

  q = xQueueCreate(20, sizeof(TrafficLight_EventId));

  xTaskCreate(taskClock, "Clock Task", 1024, NULL, 3, &hClock);
  xTaskCreate(taskTrafficLight, "TrafficLight Task", 2048, NULL, 2, &hTrafficLight);
  xTaskCreate(taskRequest, "Request Task", 1024, NULL, 1, &hRequest);
  xTaskCreate(taskSerial, "Serial Task", 4096, NULL, 1, &hSerial);
}


void loop() {}