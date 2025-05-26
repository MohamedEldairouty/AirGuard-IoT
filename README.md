<p align="center">
  <img src="Images/logo.jpg" width="200" alt="AirGuard Logo">
</p>

# AirGuard – IoT-Based Air Quality Monitoring System 🌫

**AirGuard** is a real-time indoor air quality monitoring system built using the ESP32 microcontroller, BME680 environmental sensor, and the Blynk IoT platform. It monitors key environmental parameters such as **IAQ (Indoor Air Quality Index)**, temperature, humidity, and pressure. When air quality drops below a safe threshold, the system activates a buzzer alarm and sends **push notifications** to the user’s smartphone.

---

## This File is the Project Report 📄  
For the project poster, please refer to:  
**[`Poster/poster.pdf`](Poster/poster.pdf)**

---

## Problem Statement ❓

Poor indoor air quality (IAQ) can negatively impact health, productivity, and comfort. Most people are unaware of the gradual accumulation of harmful pollutants such as VOCs (volatile organic compounds), CO₂, or excess humidity. Traditional monitoring systems are expensive or limited in scope.

---

## Proposed Solution ✅

We propose **AirGuard**, an affordable, IoT-based air monitoring system using the **BME680 sensor** and **ESP32**, with real-time notifications and a mobile dashboard via **Blynk (MQTT-based)**. This project enables proactive monitoring and timely alerts for healthier indoor environments.

---

## System Architecture 🏗

[ESP32 Microcontroller] <–> [Blynk Cloud (MQTT Protocol)] <–> [Mobile Blynk App]

| Layer              | Technology Used                    |
|--------------------|-------------------------------------|
| Microcontroller    | ESP32                              |
| Sensor             | BME680 (via I2C)                   |
| IoT Protocol       | **MQTT** (via Blynk)               |
| Mobile Interface   | Blynk App                          |
| Development Tools  | Arduino IDE + C++ + BSEC Library   |

---

## Why MQTT? 🔗

- **MQTT (Message Queuing Telemetry Transport)** is ideal for IoT due to its:
  - Low power and bandwidth consumption
  - Lightweight publish/subscribe model
  - Reliable real-time message delivery
- **Blynk** uses MQTT under the hood to simplify cloud communication between the ESP32 and the mobile dashboard.
- We chose it for its **fast prototyping**, **global internet access (WAN)**, and **robust alert features**.

---

## Features ✨

- 📡 Real-time IAQ calculation using Bosch BSEC algorithm  
- 🌡 Displays temperature, humidity, and pressure  
- 🔔 Automatic buzzer when IAQ becomes unhealthy  
- 📲 Push notifications via Blynk  
- 📟 OLED display for local readings  
- 🌐 Global (WAN-based) remote monitoring  

---

## Technologies Used 🛠

- **ESP32 Dev Board** – Main Wi-Fi-enabled microcontroller  
- **BME680 Sensor** – Gas + environment sensing  
- **Bosch BSEC Library v1.8.1492** – For IAQ estimation  
- **Blynk IoT Platform** – Cloud + mobile dashboard  
- **Arduino IDE** – Development environment  
- **C++ (Arduino Core)** – Programming language  

---

## Hardware Components 🧩

| Component          | Description                        |
|--------------------|------------------------------------|
| 🧠 ESP32 Dev Board    | Wi-Fi enabled microcontroller      |
| 🌫 BME680 Sensor      | Air quality & environment sensing  |
| 🔊 Active Buzzer      | Audio alert for poor air quality   |
| 📟 OLED Display       | Local Data Display                 |
| 🔌 Breadboard + Wires | For prototyping connections        |

---

## Screenshots 📸

### Live IAQ Monitoring via Blynk App

![App Screenshot](Images/Blynk_App_Dashboard.jpg)

### System Overview (Hardware Setup Reference)

![Hardware Setup](Images/hardware_demo.jpg)

---

## Demo Video 🎥

▶ [Click to watch the demo](Demo_Video/airguard_demo.mp4)

---

## How to Use 🧪

1. 📥 Clone this repo and upload `AirGuard_BSEC_Blynk.ino` to your ESP32 via Arduino IDE.  
2. 🛠 Connect the hardware as shown in the *hardware setup image* above.  
3. 📲 Open the Blynk app to monitor IAQ data in real-time.  
4. 🚨 If IAQ ≥ 150, buzzer sounds and you receive a push notification.  
5. 🧠 Toggle alarm manually via Blynk if needed.  

---

## Evaluation Criteria Coverage ✅

| Criteria                          | Covered In Project                            |
|----------------------------------|------------------------------------------------|
| **Running Code**                 | Fully working on real hardware                |
| **IoT Protocol Used**            | MQTT (via Blynk) – Explained above            |
| **Mobile/Web Interface**         | Blynk App with real-time sync                 |
| **WAN Scale Communication**      | Blynk cloud used over global internet         |
| **Working Maquette**             | Assembled hardware model shown in images      |
| **Poster + Report**              | Poster in `/Poster`, report = this README     |
| **Team Contribution**            | See team list below                           |

---

## Team Members 👥

- **Mohamed Abdallah** – 221001719 – [@MohamedEldairouty](https://github.com/MohamedEldairouty)  
- **Maya Hossam** – 221000302 – [@MayaMorsy](https://github.com/MayaMorsy)  
- **Habiba Amr** – 221000287 – [@HabibaGhoneim](https://github.com/HabibaGhoneim)  
- **Jana Ayman** – 221000985

---

## License 📄

This project is licensed under the MIT License.