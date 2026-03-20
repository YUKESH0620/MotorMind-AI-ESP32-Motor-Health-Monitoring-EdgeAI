/*
=====================================================
CONFIGURATION FILE
MotorMind AI v1
Contains:
  Pin definitions
  System thresholds
  WiFi credentials
  Logging modes
=====================================================
*/

#ifndef CONFIG_H
#define CONFIG_H

// ================= PIN DEFINITIONS =================
#define IR_PIN          27
#define CURRENT_PIN     34
#define ONE_WIRE_BUS    4

#define BUZZER_PIN      26
#define LED_NORMAL      18
#define LED_WARNING     19
#define LED_FAULT       33

// I2C
#define SDA_PIN 21
#define SCL_PIN 22


// ================= DISPLAY SETTINGS =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C


// ================= SENSOR THRESHOLDS =================
#define VIBRATION_THRESHOLD   0.35
#define CURRENT_THRESHOLD     0.25
#define TEMPERATURE_THRESHOLD 60.0


// ================= CURRENT SENSOR =================
#define ACS712_SENSITIVITY 0.1
#define ACS712_OFFSET      2.5


// ================= LOGGING MODES =================
#define LOG_DEBUG   1
#define LOG_INFO    2
#define LOG_DATASET 3


// ================= WIFI CONFIG =================
#define WIFI_SSID "POCO C65"
#define WIFI_PASS "12202006"


// ================= BLYNK CONFIG =================
#define BLYNK_TEMPLATE_ID "TMPL322mbuW0v"
#define BLYNK_TEMPLATE_NAME "Motor Health Monitoring"

#define BLYNK_AUTH_TOKEN "VbxTljZqaT62lplxVUMRFzPgEYrbAH2H"


// ================= SYSTEM CONSTANTS =================
#define RPM_MIN_VALID 100
#define RPM_MAX_VALID 3000

#define HEALTH_WARNING_LEVEL 75
#define HEALTH_CRITICAL_LEVEL 30

#define ABNORMAL_CONFIRM_COUNT 3


// ================= SAMPLE COUNTS =================
#define VIBRATION_SAMPLES 150
#define CURRENT_SAMPLES 500

#endif