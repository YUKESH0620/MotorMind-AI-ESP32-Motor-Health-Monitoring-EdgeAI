/*
=====================================================
BLYNK IoT MODULE
MotorMind AI v1

Handles:
  Sending sensor data to Blynk
  Dashboard updates
  Alert notifications
  Wifi Manager function
=====================================================
*/

#ifndef IOT_BLYNK_H
#define IOT_BLYNK_H

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "config.h"
#include "lcd_serial.h"

// Blynk objects
extern BlynkTimer timer;

// Data variables shared with main firmware
extern float temperature;
extern float vibration;
extern float current;
extern int motorhealth;
extern int Rpm;
extern int RUL;

extern int normalLED;
extern int warningLED;
extern int faultLED;
extern int buzzer;

extern String fault;
extern String maintenance;

extern bool alertSent;

// Wifi manager variables
unsigned long lastWifiCheck = 0;
const unsigned long wifiRetryInterval = 10000; // retry every 10 seconds
bool wifiConnected = false;


// ================= SEND DATA TO BLYNK =================
void sendDataToBlynk()
{
    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, vibration);
    Blynk.virtualWrite(V2, current);
    Blynk.virtualWrite(V3, motorhealth);
    Blynk.virtualWrite(V4, fault);
    Blynk.virtualWrite(V5, Rpm);

    Blynk.virtualWrite(V6, normalLED);
    Blynk.virtualWrite(V7, warningLED);
    Blynk.virtualWrite(V8, faultLED);
    Blynk.virtualWrite(V9, buzzer);

    Blynk.virtualWrite(V10, RUL);
    Blynk.virtualWrite(V11, maintenance);
}


// ================= FAULT ALERT =================
void checkMotorAlert(float motorHealth)
{
    if (motorHealth < HEALTH_CRITICAL_LEVEL && !alertSent)
    {
        Blynk.logEvent("motor_fault", "Motor health critical! Maintenance required.");
        alertSent = true;
    }

    if (motorHealth > 40)
    {
        alertSent = false;
    }
}


// ================= WIFI MANAGER =================
void manageWiFi()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (!wifiConnected)
        {
            wifiConnected = true;

            sendLCD("WIFI:CONNECTED");

            Blynk.config(BLYNK_AUTH_TOKEN);
            if(!Blynk.connected())
            {
               Blynk.connect();
            }

            sendLCD("BLYNK:ONLINE");
        }
    }
    else
    {
        wifiConnected = false;

        sendLCD("BLYNK:OFFLINE");

        if (millis() - lastWifiCheck > wifiRetryInterval)
        {
            lastWifiCheck = millis();

            sendLCD("WIFI:RETRY");

            WiFi.disconnect();
            WiFi.begin(WIFI_SSID, WIFI_PASS);
        }
    }
}

#endif