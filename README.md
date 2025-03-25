# ESP-IDF Project: GPS & MQTT with SIM Module and SSD1306 Display

## 📌 Overview
This project is developed using **ESP-IDF** to integrate **GPS**, **MQTT communication**, and **SSD1306 OLED display** with an ESP32. The system receives GPS data, processes it, and publishes the information via MQTT while displaying relevant data on an OLED screen.

## 🚀 Features
- **GPS Module (UART)**: Reads and parses NMEA data.
- **SIM Module (UART, MQTT)**: Publishes GPS data to an MQTT broker.
- **SSD1306 OLED Display (I2C)**: Displays real-time GPS information.
- **MQTT Communication**: Supports real-time data transmission.
- **Optimized for ESP-IDF**: Uses ESP32-S3 hardware features efficiently.

## 🛠 Hardware Requirements
- **ESP32-S3 Board**
- **SIM Module SIMCOM A7680C**
- **GPS Module NEO-7M**
- **SSD1306 OLED Display**

![a5f551d2-b508-4557-a444-56a5286aceac](https://github.com/user-attachments/assets/5786d198-5a96-4e16-b3d5-f72ae16eea5b)

![f3c2d993-286e-47ab-a0ca-d6d997ee8b1a](https://github.com/user-attachments/assets/6623039f-5f14-4c63-a71f-f22a4ca203b6)


## 📡 MQTT Configuration
- **Broker:** `tcp://broker.emqx.io:1883`
- **Publish Topic:** `project1/test1`
- **Subscribe Topic:** `project1/test`

