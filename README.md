# Traffic Management System (ESP32 / FreeRTOS)

This project implements a real-time traffic light control system on an **ESP32** using **FreeRTOS** and a state machine generated with **StateSmith**.

The application utilizes a clean **Producer-Consumer architecture**, separating business logic, timing, and user input into dedicated tasks communicating via thread-safe queues.

*Developed as part of the **Technische Informatik 2** module.*

## Hardware Setup

The project is configured for a standard **ESP32 Development Board** (e.g., ESP32-WROOM / NodeMCU).

### Pin Configuration (Wiring)

| Component | GPIO Pin | Mode | Note |
| :--- | :--- | :--- | :--- |
| **LED Red** | `2` | Output | Internal LED on some boards, or external |
| **LED Yellow** | `4` | Output | Cathode to GND, use resistor |
| **LED Green** | `5` | Output | Cathode to GND, use resistor |
| **Button** | `13` | Input Pullup | Connects to **GND** when pressed |

> **Note:** The button is configured as `INPUT_PULLUP`. It triggers the signal when pulled to ground (active low).

## Software Architecture

The system is built on **FreeRTOS** to handle multitasking efficiently.

### Tasks & Priorities

1.  **Clock Task (Prio 3):** High-priority generator that sends a `TICK` event every 1ms to the queue to ensure accurate timing.

2.  **TrafficLight Task (Prio 2):** The consumer task hosting the StateSmith logic. It processes events (`TICK`, `REQUESTGREEN`) from the queue.

3.  **Request Task (Prio 1):** Monitors the physical button (Pin 13) with debouncing logic and produces `REQUESTGREEN` events.

4.  **Serial Task (Prio 1):** Handles user interaction via UART (Serial Monitor), robust against whitespace/newline errors.

### State Machine

The main logic is generated using **StateSmith** (`TrafficLight.hpp`/`.cpp`). It implements a standard traffic light cycle:
- `TRAFFICLIGHTGREEN`
- `TRAFFICLIGHTYELLOW`
- `TRAFFICLIGHTRED`
- `TRAFFICLIGHTREDYELLOW`

## Installation & Usage

1.  **Open Project:** Open this folder in VS Code with the **PlatformIO** extension installed.

2.  **Build & Upload:** Connect your ESP32 and use the PlatformIO "Upload" button.

3.  **Monitor:** Open the Serial Monitor at **115200 baud**.

### Serial Control

You can interact with the system via the Serial Monitor:

- Type `1`: Prints the current state of the traffic light (e.g., `TRAFFICLIGHTRED`).

- Type `2`: Simulates a pedestrian button press (`REQUESTGREEN`).

## Tech Stack

-   **Hardware:** ESP32 (Espressif Systems)
-   **Framework:** Arduino (via PlatformIO)
-   **OS:** FreeRTOS (Tasks, Queues)
-   **Modeling:** StateSmith (State machine)
-   **Language:** C++

---

**Version:** 2.0