/*=============================================================================
 * IoT Indoor Plant Environment Monitor
 * Author: Daggolu Hari Krishna
 * Platform: ESP32 (Arduino Framework)
 *
 * Monitors indoor plant health using multiple environmental sensors.
 * Sends data to ThingSpeak cloud for dashboard visualization.
 * Provides local alerts for critical conditions.
 *
 * Components:
 *  - ESP32 DevKit
 *  - Soil Moisture Sensor (Capacitive)
 *  - DHT22 Temperature & Humidity Sensor
 *  - LDR Light Sensor
 *  - Water Pump Relay (auto-watering)
 *  - OLED Display (SSD1306 128x64)
 *  - Buzzer for alerts
 *=============================================================================*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

// ==================== Configuration ====================
const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ThingSpeak Configuration
const char* THINGSPEAK_API_KEY = "YOUR_WRITE_API_KEY";
const char* THINGSPEAK_URL     = "http://api.thingspeak.com/update";
const long  CLOUD_INTERVAL     = 15000; // 15 seconds (ThingSpeak free tier limit)

// ==================== Pin Definitions ====================
#define DHT_PIN           4
#define DHT_TYPE          DHT22
#define SOIL_MOISTURE_PIN 34   // ADC pin (capacitive sensor)
#define LDR_PIN           35   // ADC pin
#define WATER_PUMP_RELAY  16   // Relay for water pump
#define BUZZER_PIN        26
#define STATUS_LED        2

// OLED Display
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDR     0x3C

// ==================== Thresholds ====================
#define SOIL_DRY       70    // Below this % = dry
#define SOIL_CRITICAL  30    // Below this % = critical
#define SOIL_WET       85    // Above this % = too wet
#define TEMP_HIGH      35.0  // Too hot for most plants
#define TEMP_LOW       10.0  // Too cold
#define HUMIDITY_LOW   30.0  // Too dry air
#define LIGHT_LOW      20    // Too dark (%)
#define LIGHT_HIGH     90    // Too bright (%)

// Auto-watering
#define WATERING_DURATION  5000   // 5 seconds pump ON
#define WATERING_COOLDOWN  300000 // 5 min between watering

// ==================== Objects ====================
DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ==================== Global Variables ====================
float temperature     = 0.0;
float humidity        = 0.0;
int   soilMoistureRaw = 0;
float soilMoisturePct = 0.0;
int   lightRaw        = 0;
float lightPct        = 0.0;

unsigned long lastSensorRead  = 0;
unsigned long lastCloudPush   = 0;
unsigned long lastWatering    = 0;
bool waterPumpActive          = false;
unsigned long pumpStartTime   = 0;

const long SENSOR_INTERVAL = 2000;

// Plant health status
enum PlantStatus {
    PLANT_HAPPY,
    PLANT_NEEDS_WATER,
    PLANT_CRITICAL,
    PLANT_OVERWATERED,
    PLANT_TOO_HOT,
    PLANT_TOO_COLD
};
PlantStatus plantStatus = PLANT_HAPPY;

// Display page cycling
int displayPage = 0;
unsigned long lastPageChange = 0;
const long PAGE_INTERVAL = 3000;

// ==================== Setup ====================
void setup() {
    Serial.begin(115200);
    Serial.println(F("\n========================================"));
    Serial.println(F("  Indoor Plant Environment Monitor"));
    Serial.println(F("  Author: Daggolu Hari Krishna"));
    Serial.println(F("========================================\n"));

    // Pin setup
    pinMode(WATER_PUMP_RELAY, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(STATUS_LED, OUTPUT);
    digitalWrite(WATER_PUMP_RELAY, HIGH); // Pump OFF (active LOW)

    // Sensor init
    dht.begin();

    // OLED init
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println(F("[ERR] OLED init failed!"));
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Splash screen
    display.setTextSize(1);
    display.setCursor(10, 10);
    display.println(F("Plant Monitor"));
    display.setCursor(10, 30);
    display.println(F("Hari Krishna"));
    display.setCursor(10, 50);
    display.println(F("Initializing..."));
    display.display();

    // WiFi
    setupWiFi();
    delay(1000);

    Serial.println(F("[OK] System initialized!"));
}

// ==================== Main Loop ====================
void loop() {
    unsigned long now = millis();

    // Read sensors
    if (now - lastSensorRead >= SENSOR_INTERVAL) {
        readSensors();
        evaluatePlantHealth();
        handleAutoWatering();
        handleAlerts();
        lastSensorRead = now;
    }

    // Update display (cycle pages)
    if (now - lastPageChange >= PAGE_INTERVAL) {
        displayPage = (displayPage + 1) % 3;
        lastPageChange = now;
    }
    updateDisplay();

    // Push to cloud
    if (now - lastCloudPush >= CLOUD_INTERVAL) {
        pushToThingSpeak();
        lastCloudPush = now;
    }

    // Handle pump duration
    if (waterPumpActive && (now - pumpStartTime >= WATERING_DURATION)) {
        stopWatering();
    }

    // WiFi reconnect
    if (WiFi.status() != WL_CONNECTED) {
        setupWiFi();
    }
}

// ==================== WiFi ====================
void setupWiFi() {
    Serial.print(F("[WiFi] Connecting..."));
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println(F(" Connected!"));
        Serial.println("[WiFi] IP: " + WiFi.localIP().toString());
        digitalWrite(STATUS_LED, HIGH);
    } else {
        Serial.println(F(" Failed!"));
    }
}

// ==================== Sensor Reading ====================
void readSensors() {
    // DHT22
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    if (isnan(temperature)) temperature = 0;
    if (isnan(humidity)) humidity = 0;

    // Soil moisture (capacitive: higher value = drier)
    soilMoistureRaw = analogRead(SOIL_MOISTURE_PIN);
    // Map: 4095 (dry/air) → 0%, 1500 (water) → 100%
    soilMoisturePct = map(soilMoistureRaw, 4095, 1500, 0, 100);
    soilMoisturePct = constrain(soilMoisturePct, 0, 100);

    // Light sensor
    lightRaw = analogRead(LDR_PIN);
    lightPct = map(lightRaw, 0, 4095, 0, 100);
    lightPct = constrain(lightPct, 0, 100);

    // Serial log
    Serial.printf("[SENSOR] Temp=%.1f°C | Hum=%.1f%% | Soil=%d%% | Light=%d%%\n",
                  temperature, humidity, (int)soilMoisturePct, (int)lightPct);
}

// ==================== Plant Health ====================
void evaluatePlantHealth() {
    if (soilMoisturePct < SOIL_CRITICAL) {
        plantStatus = PLANT_CRITICAL;
    } else if (soilMoisturePct > SOIL_WET) {
        plantStatus = PLANT_OVERWATERED;
    } else if (soilMoisturePct < SOIL_DRY) {
        plantStatus = PLANT_NEEDS_WATER;
    } else if (temperature > TEMP_HIGH) {
        plantStatus = PLANT_TOO_HOT;
    } else if (temperature < TEMP_LOW) {
        plantStatus = PLANT_TOO_COLD;
    } else {
        plantStatus = PLANT_HAPPY;
    }
}

// ==================== Auto Watering ====================
void handleAutoWatering() {
    unsigned long now = millis();

    if (soilMoisturePct < SOIL_DRY &&
        !waterPumpActive &&
        (now - lastWatering >= WATERING_COOLDOWN)) {

        startWatering();
    }
}

void startWatering() {
    Serial.println(F("[PUMP] Starting auto-watering..."));
    digitalWrite(WATER_PUMP_RELAY, LOW); // Active LOW
    waterPumpActive = true;
    pumpStartTime = millis();
}

void stopWatering() {
    Serial.println(F("[PUMP] Watering complete."));
    digitalWrite(WATER_PUMP_RELAY, HIGH);
    waterPumpActive = false;
    lastWatering = millis();
}

// ==================== Alerts ====================
void handleAlerts() {
    switch (plantStatus) {
        case PLANT_CRITICAL:
            tone(BUZZER_PIN, 2000, 200);
            break;
        case PLANT_NEEDS_WATER:
        case PLANT_TOO_HOT:
        case PLANT_TOO_COLD:
            tone(BUZZER_PIN, 1000, 100);
            break;
        default:
            noTone(BUZZER_PIN);
            break;
    }
}

// ==================== OLED Display ====================
void updateDisplay() {
    display.clearDisplay();
    display.setTextSize(1);

    switch (displayPage) {
        case 0: // Overview
            display.setCursor(0, 0);
            display.println(F("=== PLANT MONITOR ==="));
            display.println();
            display.printf("Temp:  %.1f C\n", temperature);
            display.printf("Humid: %.1f %%\n", humidity);
            display.printf("Soil:  %d %%\n", (int)soilMoisturePct);
            display.printf("Light: %d %%\n", (int)lightPct);
            break;

        case 1: // Plant status
            display.setCursor(0, 0);
            display.println(F("=== PLANT STATUS ==="));
            display.println();
            display.setTextSize(1);
            switch (plantStatus) {
                case PLANT_HAPPY:
                    display.println(F("  Status: HAPPY!"));
                    display.println(F("  All conditions"));
                    display.println(F("  are optimal."));
                    break;
                case PLANT_NEEDS_WATER:
                    display.println(F("  NEEDS WATER!"));
                    display.printf("  Soil: %d%%\n", (int)soilMoisturePct);
                    break;
                case PLANT_CRITICAL:
                    display.println(F("  !! CRITICAL !!"));
                    display.println(F("  Soil very dry!"));
                    display.println(F("  Watering now..."));
                    break;
                case PLANT_OVERWATERED:
                    display.println(F("  OVERWATERED!"));
                    display.println(F("  Reduce watering"));
                    break;
                case PLANT_TOO_HOT:
                    display.println(F("  TOO HOT!"));
                    display.printf("  Temp: %.1f C\n", temperature);
                    break;
                case PLANT_TOO_COLD:
                    display.println(F("  TOO COLD!"));
                    display.printf("  Temp: %.1f C\n", temperature);
                    break;
            }
            break;

        case 2: // Network info
            display.setCursor(0, 0);
            display.println(F("=== NETWORK INFO ==="));
            display.println();
            if (WiFi.status() == WL_CONNECTED) {
                display.println(F("WiFi: Connected"));
                display.print(F("IP: "));
                display.println(WiFi.localIP());
                display.println(F("Cloud: ThingSpeak"));
                display.println(F("Status: Sending"));
            } else {
                display.println(F("WiFi: Disconnected"));
                display.println(F("Reconnecting..."));
            }
            break;
    }

    // Pump indicator
    if (waterPumpActive) {
        display.setCursor(90, 56);
        display.print(F("[PUMP]"));
    }

    display.display();
}

// ==================== ThingSpeak ====================
void pushToThingSpeak() {
    if (WiFi.status() != WL_CONNECTED) return;

    HTTPClient http;
    String url = String(THINGSPEAK_URL) +
                 "?api_key=" + THINGSPEAK_API_KEY +
                 "&field1=" + String(temperature, 1) +
                 "&field2=" + String(humidity, 1) +
                 "&field3=" + String((int)soilMoisturePct) +
                 "&field4=" + String((int)lightPct) +
                 "&field5=" + String(waterPumpActive ? 1 : 0);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
        Serial.printf("[CLOUD] ThingSpeak update: %d\n", httpCode);
    } else {
        Serial.printf("[CLOUD] Error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}
