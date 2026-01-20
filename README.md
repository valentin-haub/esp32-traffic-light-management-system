# Traffic Management System (ESP32 / FreeRTOS)

Dieses Projekt implementiert eine Echtzeit-Ampelsteuerung für **zwei unabhängige Kreuzungen** auf einem einzelnen **ESP32** unter Verwendung von **FreeRTOS** und mit **StateSmith** generierten Zustandsautomaten.

Das System verfügt über eine saubere **Producer-Consumer-Architektur**, trennt Ampel-Logik, Timing und Benutzereingaben in dedizierte Tasks und visualisiert Statusinformationen über ein **OLED-Display** sowie eine **binäre LED-Anzeige**.

*Entwickelt als Teil des Moduls **Technische Informatik 2**.*

## Features

- **Duale Ampelsteuerung:** Zwei unabhängige Zustandsautomaten laufen parallel.
- **Smarter Bahnübergang (Neu):**
  - **Schrankensteuerung:** Ein Servo-Motor wird durch Neigen eines MPU6050 Beschleunigungssensors ("Hebel") gesteuert.
  - **Sicherheits-Logik:** Die Ampel darf erst Grün werden, wenn die Schranke physisch geschlossen ist (< 5°) **UND** eine Sicherheitszeit von 4 Sekunden abgelaufen ist.
  - **Interlock:** Der Servo lässt sich nur bewegen, wenn die Ampel Rot zeigt.
- **OLED Status-Display (Neu):** Zeigt Echtzeit-Informationen an:
  - Aktuelle Neigung & Servowinkel.
  - Status der Schranke (OFFEN / GESCHLOSSEN).
  - Countdown-Timer für die Sicherheitsfreigabe.
- **Binäre Anzeige:** 4 LEDs zeigen die aktuell eingestellte Grünphasen-Dauer (1-10s) binär an (0-15).
- **Nachtmodus:** Automatisches Gelb-Blinken basierend auf dem Helligkeitssensor (LDR).
- **Konfigurierbares Timing:** Ein Potentiometer passt die Grünphasendauer in Echtzeit an.
- **Interaktives Serielles Menü:** Überwachung und Steuerung via UART.

## Hardware Setup

Das Projekt ist für ein Standard **ESP32 Development Board** konfiguriert.

### Pin-Konfiguration (Verkabelung)

| Komponente | GPIO Pin | Hinweis |
| :--- | :--- | :--- |
| **Ampel 1** | **Rot:** `23`, **Gelb:** `22`, **Grün:** `21` | |
| **Ampel 2** | **Rot:** `18`, **Gelb:** `5`, **Grün:** `17` | |
| **Taster 1** (Request TL1) | `19` | Active Low (Input Pullup) |
| **Taster 2** (Request TL2) | `16` | Active Low (Input Pullup) |
| **I2C Bus** (OLED & MPU6050) | **SDA:** `27`, **SCL:** `14` | |
| **Servo Motor** | `15` | PWM Signal |
| **Binär-Anzeige (LSB)** | `32` | Bit 0 (Wert 1) |
| **Binär-Anzeige** | `33` | Bit 1 (Wert 2) |
| **Binär-Anzeige** | `25` | Bit 2 (Wert 4) |
| **Binär-Anzeige (MSB)** | `26` | Bit 3 (Wert 8) |
| **LDR (Lichtsensor)** | `12` | Analog Input |
| **Potentiometer** | `13` | Analog Input |

## Simulation (Wokwi)

Dieses Projekt enthält eine vollständig konfigurierte **Wokwi Simulation**.

**Starten der Simulation:**
1. Installiere die **Wokwi Simulator** Extension in VS Code.
2. **Projekt Bauen:** Klicke auf das **Build** Icon (`✔`) in PlatformIO.
3. Öffne `simulation/diagram.json`.
4. Drücke `F1` und wähle **"Wokwi: Start Simulator"**.

*Hinweis: Um die Simulationsgeschwindigkeit zu optimieren, läuft der I2C-Bus im Fast-Mode (400kHz) und das Display wird mit 10Hz aktualisiert.*

## Software Architektur

Das System nutzt **FreeRTOS** für Multitasking. Zur Gewährleistung der "Separation of Concerns" hat jede Ampel ihren eigenen Task und ihre eigene Queue.

### Tasks & Prioritäten

1. **Clock Task (Prio 3):** Generiert `TICK` Events alle 1ms und verteilt sie an *beide* Queues (`q1`, `q2`).
2. **TrafficLight Tasks 1 & 2 (Prio 2):** Zwei separate Consumer-Tasks, welche die StateSmith-Logik ausführen.
3. **Sensors Task (Prio 1):**
   - Liest analoge Sensoren (LDR, Poti).
   - Liest den **MPU6050** (Neigung) und steuert den **Servo**.
   - Berechnet die **Sicherheits-Guard-Logik** (4s Timer).
   - Aktualisiert das **OLED-Display** und die **Binär-LEDs** über den Treiber.
4. **Request Tasks 1 & 2 (Prio 1):** Überwachen die Taster (Debouncing) und senden `REQUESTGREEN` Events.
5. **Serial Task (Prio 1):** Behandelt Benutzereingaben via UART.

### State Machine

Die Logik wird mittels **StateSmith** (`TrafficLight.hpp`/`.cpp`) generiert. Beide Ampel-Instanzen (`tl1`, `tl2`) teilen sich die gleiche Logikstruktur, arbeiten aber auf unterschiedlichen Datensätzen.

Die Transition zu Grün wird durch die Variable `barrierActive` geschützt, welche nur `true` wird, wenn die Schranke sicher geschlossen ist.

## Bedienung / Serielles Menü

Verbinde dich mit dem Serial Monitor bei **115200 Baud**.

- **1:** Aktuelle Zustände ausgeben (Zeigt "NIGHTMODE" falls aktiv).
- **2:** Fußgängeranforderung für Ampel 1 oder 2 simulieren.
- **3:** Aktuelle Dauer der Grünphase anzeigen (in Sekunden).
- **4:** Aktuellen Helligkeitswert anzeigen.

---

**Version:** 5.0 (Barrier-System & OLED Update)