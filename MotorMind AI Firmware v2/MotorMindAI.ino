/*
=====================================================
MotorMind AI v1
Edge AI Motor Health Monitoring System
Author: Yukesh

Main Firmware
=====================================================
*/

#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"
#include "sensors.h"
#include "features.h"
#include "ai_engine.h"
#include "health_model.h"
#include "display_oled.h"

#include "iot_blynk.h"
#include "alerts.h"
#include "motor_model.h"
#include "logger.h"
#include "calibration.h"

// ================= GLOBAL OBJECTS =================
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Eloquent::ML::Port::DecisionTree model;

// ================= GLOBAL VARIABLES =================
volatile unsigned long pulseCount = 0;
volatile unsigned long lastPulseTime = 0;

float filteredRPM = 0;
float previousHealth = 100;
float offsetVoltage = ACS712_OFFSET;

int abnormalCount = 0;
String fault = "NORMAL";

BlynkTimer timer;

float temperature = 0;
float vibration = 0;
float current = 0;
int motorhealth = 0;
int Rpm = 0;
int RUL = 100;

int normalLED = 0;
int warningLED = 0;
int faultLED = 0;
int buzzer = 0;

String maintenance;
bool alertSent = false;

int logMode = LOG_DEBUG;


// ================= SENSOR DATA ==================
float vibMean, vibRMS, vibStdDev;
float currentMean, currentRMS, currentStdDev;

float rpm = 0;
float tempC = 0;

float motorHealth = 100;
int aiPrediction = 0;

float features[8];
MotorFeatures f;

unsigned long rpmTimer = 0;
const int RPM_INTERVAL = 1000;



// ================= SETUP =================
void setup()
{
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, -1, 25);

    Wire.begin(SDA_PIN, SCL_PIN);

    pinMode(LED_NORMAL, OUTPUT);
    pinMode(LED_WARNING, OUTPUT);
    pinMode(LED_FAULT, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    pinMode(IR_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(IR_PIN), countPulse, RISING);

    analogReadResolution(12);
    analogSetPinAttenuation(CURRENT_PIN, ADC_11db);

    // WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    delay(2000);
    manageWiFi();

    // Blynk timer
    timer.setInterval(2000L, sendDataToBlynk);

    // Sensors
    accel.begin();
    accel.setRange(ADXL345_RANGE_2_G);
    tempSensor.begin();

    // ACS712 Calibration
    analogReadResolution(12);
    analogSetPinAttenuation(CURRENT_PIN, ADC_11db);
    calibrateCurrentSensor();

    // OLED
    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);

    showStartupScreen();
}


// ================= MAIN LOOP =================
void loop()
{
  
    
    if (wifiConnected)
    {
       Blynk.run();
    }
    timer.run();

    // ================= SENSOR TASK =================

        readVibration(vibMean, vibRMS, vibStdDev);
        readCurrent(currentMean, currentRMS, currentStdDev);

        rpm = readRPM();
        tempC = readTemperature();
       

   // ================= AI TASK =================

        f.vibRMS = vibRMS;
        f.vibMean = vibMean;
        f.vibStdDev = vibStdDev;

        f.currentRMS = currentRMS;
        f.currentMean = currentMean;
        f.currentStdDev = currentStdDev;

        f.rpm = rpm;
        f.temperature = tempC;

        buildFeatureArray(f, features);

        // ===== AI INFERENCE =====
        aiPrediction = runAI(features);
        fault = updateFaultState(aiPrediction, rpm);

        // ===== MOTOR HEALTH =====
        motorHealth = computeMotorHealth(vibStdDev, currentRMS, tempC);

        RUL = estimateRUL(motorHealth);
        maintenance = maintenanceAdvice(RUL);


    // ================= DISPLAY TASK =================

        bool warningState = checkWarningState(motorHealth);
        updateAlerts(fault, warningState);

        // ===== OLED =====
       if(fault == "STOPPED")
            showStoppedScreen();
       else
            displayMotorData(vibRMS, currentRMS, rpm, tempC, motorHealth, fault);

       // ===== DATA FOR IOT =====
       temperature = tempC;
       vibration = vibRMS;
       current = currentRMS;
       Rpm = rpm;
       motorhealth = motorHealth;

       // ===== ALERT NOTIFICATION =====
       checkMotorAlert(motorHealth);

      // ======LCD Through Arduino UNO======
       if(motorHealth < 30 )
            sendLCD("MAINTENANCE!");

       else if(fault == "UNBALANCE")
            sendLCD("FAULT:UNBALANCE");

       else if(fault == "OVERCURRENT")
            sendLCD("FAULT:OVERCURRENT");

       else if(fault == "OVERHEAT")
            sendLCD("FAULT:OVERHEAT");

       else if(fault == "STOPPED")
            sendLCD("STATUS:STOPPED");

       else
            sendLCD("STATUS:NORMAL");



    // ================= LOGGER TASK =================

        runLogger(
            vibRMS,
            vibMean,
            vibStdDev,
            currentRMS,
            currentMean,
            currentStdDev,
            rpm,
            tempC,
            motorHealth,
            aiPrediction,
            features,
            fault
        );


}