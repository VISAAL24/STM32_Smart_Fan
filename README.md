# 🌬️ STM32 Smart Fan Controller

A smart temperature-controlled fan system built using **STM32F411**, featuring real-time temperature & humidity monitoring, automatic fan speed control (PWM), and OLED display output.

---

## 🚀 Features

* 🌡️ **Temperature & Humidity Monitoring** using DHT11
* 🌀 **Automatic Fan Speed Control** based on temperature
* ⚡ **PWM Motor Control** using TIM3
* 📟 **OLED Display (SSD1306 - I2C)**
* 💡 **LED Status Indicator**
* 🔌 **UART Debug Output** (115200 baud)
* 🎬 **Startup UI Screens with Animations & Emojis**

---

## 🛠️ Hardware Used

* STM32F411RE (Nucleo / Black Pill)
* DHT11 Temperature Sensor
* SSD1306 OLED Display (I2C - 128x64)
* DC Motor + L293D Driver
* LED
* Jumper wires & breadboard

---

## 🔌 Pin Configuration

| Peripheral       | Pin            |
| ---------------- | -------------- |
| DHT11 Data       | PA1            |
| PWM Output (Fan) | PA6 (TIM3 CH1) |
| Motor Direction  | PA7, PA8       |
| LED              | PA5            |
| UART TX/RX       | PA2 / PA3      |
| I2C SCL          | PB6            |
| I2C SDA          | PB7            |

---

## ⚙️ How It Works

1. DHT11 reads temperature & humidity
2. STM32 processes temperature
3. Fan speed is adjusted using PWM:

| Temperature | Fan Speed |
| ----------- | --------- |
| < 20°C      | OFF       |
| 20–23°C     | LOW       |
| 23–25°C     | MED       |
| 25–27°C     | HIGH      |
| > 27°C      | MAX       |

4. OLED displays:

   * Temperature
   * Humidity
   * Fan Status

---

## 🖥️ OLED Display Layout

```
TEMP :   25°C
HUM  :   60%
FAN  :   MED
```

---

## 📦 Libraries Used

* STM32 HAL Drivers
* SSD1306 OLED Library
* Fonts Library

---

## ▶️ Build & Run

1. Open project in **STM32CubeIDE**
2. Build project
3. Flash to STM32 board
4. Open Serial Monitor (115200 baud)

---

## 📸 Demo Flow

1. Welcome Screen 😊
2. Project Title (STM32 Smart Fan)
3. Starting Screen
4. Live Data Display

---

## 🧠 Future Improvements

* 🔄 Add FreeRTOS for multitasking
* 📶 Add WiFi (ESP8266 / ESP32)
* 📱 Mobile App Monitoring
* 🌡️ Use more accurate sensor (DHT22 / BME280)
* 🔊 Add buzzer alerts

---

## ⭐ If you like this project

Give it a ⭐ on GitHub and share it!

---
