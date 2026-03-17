<p align="center">
  <img src="https://img.shields.io/badge/Platform-ESP32-blue?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP32"/>
  <img src="https://img.shields.io/badge/Cloud-ThingSpeak-orange?style=for-the-badge" alt="ThingSpeak"/>
  <img src="https://img.shields.io/badge/Display-OLED-blueviolet?style=for-the-badge" alt="OLED"/>
  <img src="https://img.shields.io/badge/Category-IoT-green?style=for-the-badge" alt="IoT"/>
</p>

# 🌱 IoT Indoor Plant Environment Monitor

> An ESP32-based smart plant monitoring system with soil moisture sensing, auto-watering, OLED display, and ThingSpeak cloud dashboard — complete IoT pipeline from sensor to cloud.

---

## 🔍 Overview

This project creates a **complete IoT pipeline** for indoor plant health monitoring. It reads environmental data from multiple sensors, evaluates plant health using configurable thresholds, triggers auto-watering when soil is dry, and pushes all data to **ThingSpeak** for cloud visualization and alerting.

### Key Highlights
- 🌡️ **4 Sensors** — Soil moisture, temperature, humidity, light
- 💧 **Auto-Watering** — Pump activates when soil dries out
- 📺 **OLED Display** — 3-page cycling dashboard (128x64)
- ☁️ **ThingSpeak Cloud** — Real-time graphs and analytics
- 🌿 **Plant Health AI** — 6 status levels (Happy → Critical)
- 🔔 **Audio Alerts** — Buzzer for critical conditions
- ⏱️ **Watering Cooldown** — 5-minute minimum between watering cycles

---

## 🏗️ System Architecture

```
┌──────────────────┐
│    SENSORS       │
│  ┌────────────┐  │     ┌─────────────────────┐     ┌───────────────┐
│  │Soil Moist. ├──┼────►│     ESP32            │────►│  ThingSpeak   │
│  │DHT22       ├──┤     │                     │     │  Cloud        │
│  │LDR         ├──┤     │  ┌───────────────┐  │     │  ┌─────────┐  │
│  └────────────┘  │     │  │Health Evaluator│  │     │  │Dashboard│  │
└──────────────────┘     │  │  HAPPY         │  │     │  │& Graphs │  │
                         │  │  NEEDS_WATER   │  │     │  └─────────┘  │
┌──────────────────┐     │  │  CRITICAL      │  │     └───────────────┘
│    ACTUATORS     │     │  │  OVERWATERED   │  │
│  ┌────────────┐  │     │  │  TOO_HOT       │  │     ┌───────────────┐
│  │Water Pump  ├──┼◄────│  │  TOO_COLD      │  │────►│  OLED Display │
│  │Buzzer      ├──┤     │  └───────────────┘  │     │  (3 pages)    │
│  └────────────┘  │     └─────────────────────┘     └───────────────┘
└──────────────────┘
```

---

## 🔌 Circuit Connections

| Component | ESP32 Pin | Description |
|:---------:|:---------:|:------------|
| DHT22 | GPIO 4 | Temperature & Humidity |
| Soil Moisture | GPIO 34 (ADC) | Capacitive soil sensor |
| LDR | GPIO 35 (ADC) | Light level |
| Water Pump Relay | GPIO 16 | Auto-watering control |
| Buzzer | GPIO 26 | Audio alerts |
| Status LED | GPIO 2 | WiFi status |
| OLED SDA | GPIO 21 | I2C data |
| OLED SCL | GPIO 22 | I2C clock |

---

## 🌿 Plant Health Assessment

| Status | Condition | Action |
|:------:|:----------|:-------|
| 😊 **Happy** | All normal | None |
| 💧 **Needs Water** | Soil < 70% | Auto-water + alert |
| 🚨 **Critical** | Soil < 30% | Urgent water + alarm |
| 🌊 **Overwatered** | Soil > 85% | Warning alert |
| 🔥 **Too Hot** | Temp > 35°C | Alert |
| 🥶 **Too Cold** | Temp < 10°C | Alert |

---

## 📁 File Structure

```
IoT-Indoor-Plant-Monitor/
├── src/
│   └── main.cpp
├── docs/
├── platformio.ini
├── .gitignore
├── LICENSE
└── README.md
```

---

## 🚀 Getting Started

```bash
git clone https://github.com/HariKrishna0088/IoT-Indoor-Plant-Monitor.git
cd IoT-Indoor-Plant-Monitor

# Edit WiFi & ThingSpeak credentials in src/main.cpp
# Build & Upload
pio run --target upload

# Monitor serial output
pio device monitor
```

---

## 👨‍💻 Author

**Daggolu Hari Krishna** — B.Tech ECE | JNTUA College of Engineering, Kalikiri

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-blue?style=flat-square&logo=linkedin)](https://linkedin.com/in/harikrishnadaggolu)
[![GitHub](https://img.shields.io/badge/GitHub-HariKrishna0088-black?style=flat-square&logo=github)](https://github.com/HariKrishna0088)

---

<p align="center">⭐ Star this repo if you found it useful! ⭐</p>
