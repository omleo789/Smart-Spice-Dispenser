# Smart Spice Dispenser System 🧂⚙️

An automated, dual-controller smart kitchen appliance designed to dispense precise quantities of spices. This system utilizes a master-slave architecture with an 8051 microcontroller handling weight calculation/user interface, and an Arduino managing the servo-driven dispensing mechanism.

[![Demo Video](https://img.shields.io/badge/YouTube-Watch_Demo-red?style=for-the-badge&logo=youtube)](https://youtu.be/5gplI4gVlc8)

## 📖 Overview
Manual spice dispensing often leads to inaccurate measurements and inconsistent taste[cite: 1]. This project solves that by integrating a real-time feedback loop[cite: 1]. A load cell constantly monitors the weight, while the 8051 microcontroller processes the data and sends UART commands to an Arduino to control 5 independent servo motors, automating the dispensing process[cite: 1].

### Key Features
* **Dual-Controller Architecture:** 8051 (Master) for precise weight ADC processing and Arduino (Slave) for PWM servo control[cite: 1].
* **Real-Time Weight Feedback:** Uses an HX711 24-bit ADC and a load cell to monitor dispensed weight continuously[cite: 1].
* **Live Tare & Calibration:** Dedicated hardware button for zeroing the scale and calibrating against known weights.
* **Multi-Channel Dispensing:** 5 separate storage units controlled via independent push-buttons (Channels A-E)[cite: 1].
* **I2C Display Integration:** Real-time system status and weight output on an LCD[cite: 1].

## 🛠️ Hardware Requirements
* **Microcontrollers:** 8051 (AT89S52 or similar), Arduino Uno/Nano[cite: 1]
* **Sensors/Modules:** 
  * Load Cell (0-1kg range)[cite: 1]
  * HX711 Load Cell Module[cite: 1]
  * I2C LCD Display[cite: 1]
* **Actuators:** 5x Servo Motors[cite: 1]
* **Components:** Push Buttons (A, B, C, D, E, TARE), connecting wires, power supply (5V)[cite: 1].

## 🔀 System Architecture & Pin Mapping

### 8051 Master (Data & UI)
* **HX711:** `DT -> P3.2`, `SCK -> P3.3`
* **I2C LCD:** `SDA -> P2.0`, `SCL -> P2.1`
* **Push Buttons:** `TARE -> P1.5`, `Ch A-E -> P1.0, P1.1, P1.2, P1.3, P1.4`[cite: 1]
* **UART Tx:** Transmits signals (`'A'`, `'B'`, `'C'`, `'D'`, `'E'`) at 9600 baud to the Arduino[cite: 1].

### Arduino Slave (Actuation)
* **UART Rx:** Receives commands on `Pin 2 (RX)`[cite: 1].
* **Servos:** Attached to Pins `8`, `9`, `10`, `11`, `12`[cite: 1].

## 🚀 Working Methodology
1. **Initialization & Calibration:** The 8051 boots up the LCD, UART, and HX711[cite: 1]. The user places an empty container and presses `TARE` to zero the scale, followed by a known calibration weight.
2. **Command Input:** The user selects a spice channel (A-E) via push buttons[cite: 1]. 
3. **Communication:** The 8051 transmits the corresponding character over UART[cite: 1].
4. **Dispensing:** The Arduino receives the UART command, attaches the designated servo, and executes a 0-180 degree sweep to release the spice[cite: 1].
5. **Feedback:** The HX711 continuously reads the weight[cite: 1]. Once the target weight is reached, the flow stops[cite: 1].

## 👤 Inventor
* Om Patil[cite: 1]

---
*Note: This repository contains the source code, hardware diagrams, and logic flow for the inventive disclosure of the Smart Spice Dispenser System.*
