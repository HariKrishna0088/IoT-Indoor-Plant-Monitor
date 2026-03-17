<p align="center">
  <img src="https://img.shields.io/badge/Platform-ESP32-blue?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP32"/>
  <img src="https://img.shields.io/badge/Cloud-ThingSpeak-orange?style=for-the-badge" alt="ThingSpeak"/>
  <img src="https://img.shields.io/badge/Display-OLED-blueviolet?style=for-the-badge" alt="OLED"/>
  <img src="https://img.shields.io/badge/Category-IoT-green?style=for-the-badge" alt="IoT"/>
</p>

# ðŸŒ± IoT Indoor Plant Environment Monitor

> An ESP32-based smart plant monitoring system with soil moisture sensing, auto-watering, OLED display, and ThingSpeak cloud dashboard â€” complete IoT pipeline from sensor to cloud.

---

## ðŸ” Overview

This project creates a **complete IoT pipeline** for indoor plant health monitoring. It reads environmental data from multiple sensors, evaluates plant health using configurable thresholds, triggers auto-watering when soil is dry, and pushes all data to **ThingSpeak** for cloud visualization and alerting.

### Key Highlights
- ðŸŒ¡ï¸ **4 Sensors** â€” Soil moisture, temperature, humidity, light
- ðŸ’§ **Auto-Watering** â€” Pump activates when soil dries out
- ðŸ“º **OLED Display** â€” 3-page cycling dashboard (128x64)
- â˜ï¸ **ThingSpeak Cloud** â€” Real-time graphs and analytics
- ðŸŒ¿ **Plant Health AI** â€” 6 status levels (Happy â†’ Critical)
- ðŸ”” **Audio Alerts** â€” Buzzer for critical conditions
- â±ï¸ **Watering Cooldown** â€” 5-minute minimum between watering cycles

---

## ðŸ—ï¸ System Architecture

```
â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”
â”‚    SENSORS       â”‚
â”‚  â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”  â”‚     â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”     â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”
â”‚  â”‚Soil Moist. â”œâ”€â”€â”¼â”€â”€â”€â”€â–ºâ”‚     ESP32            â”‚â”€â”€â”€â”€â–ºâ”‚  ThingSpeak   â”‚
â”‚  â”‚DHT22       â”œâ”€â”€â”¤     â”‚                     â”‚     â”‚  Cloud        â”‚
â”‚  â”‚LDR         â”œâ”€â”€â”¤     â”‚  â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”  â”‚     â”‚  â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”  â”‚
â”‚  â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜  â”‚     â”‚  â”‚Health Evaluatorâ”‚  â”‚     â”‚  â”‚Dashboardâ”‚  â”‚
â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜     â”‚  â”‚  HAPPY         â”‚  â”‚     â”‚  â”‚& Graphs â”‚  â”‚
                         â”‚  â”‚  NEEDS_WATER   â”‚  â”‚     â”‚  â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜  â”‚
â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”     â”‚  â”‚  CRITICAL      â”‚  â”‚     â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜
â”‚    ACTUATORS     â”‚     â”‚  â”‚  OVERWATERED   â”‚  â”‚
â”‚  â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”  â”‚     â”‚  â”‚  TOO_HOT       â”‚  â”‚     â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”
â”‚  â”‚Water Pump  â”œâ”€â”€â”¼â—„â”€â”€â”€â”€â”‚  â”‚  TOO_COLD      â”‚  â”‚â”€â”€â”€â”€â–ºâ”‚  OLED Display â”‚
â”‚  â”‚Buzzer      â”œâ”€â”€â”¤     â”‚  â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜  â”‚     â”‚  (3 pages)    â”‚
â”‚  â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜  â”‚     â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜     â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜
â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜
```

---

## ðŸ”Œ Circuit Connections

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

## ðŸŒ¿ Plant Health Assessment

| Status | Condition | Action |
|:------:|:----------|:-------|
| ðŸ˜Š **Happy** | All normal | None |
| ðŸ’§ **Needs Water** | Soil < 70% | Auto-water + alert |
| ðŸš¨ **Critical** | Soil < 30% | Urgent water + alarm |
| ðŸŒŠ **Overwatered** | Soil > 85% | Warning alert |
| ðŸ”¥ **Too Hot** | Temp > 35Â°C | Alert |
| ðŸ¥¶ **Too Cold** | Temp < 10Â°C | Alert |

---

## ðŸ“ File Structure

```
IoT-Indoor-Plant-Monitor/
â”œâ”€â”€ src/
â”‚   â””â”€â”€ main.cpp
â”œâ”€â”€ docs/
â”œâ”€â”€ platformio.ini
â”œâ”€â”€ .gitignore
â”œâ”€â”€ LICENSE
â””â”€â”€ README.md
```

---

## ðŸš€ Getting Started

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

## ðŸ‘¨â€ðŸ’» Author

**Daggolu Hari Krishna** â€” B.Tech ECE | JNTUA College of Engineering, Kalikiri

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-blue?style=flat-square&logo=linkedin)](https://www.linkedin.com/in/contacthari88/)
[![GitHub](https://img.shields.io/badge/GitHub-HariKrishna0088-black?style=flat-square&logo=github)](https://github.com/HariKrishna0088)

---

<p align="center">â­ Star this repo if you found it useful! â­</p>
