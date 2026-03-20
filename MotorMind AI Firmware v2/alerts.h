/*
=====================================================
ALERT SYSTEM MODULE
MotorMind AI v1

Handles:
  LED status indicators
  Buzzer activation
  Warning state logic
=====================================================
*/

#ifndef ALERTS_H
#define ALERTS_H

#include "config.h"

// shared variables
extern int normalLED;
extern int warningLED;
extern int faultLED;
extern int buzzer;


// ================= WARNING CHECK =================
bool checkWarningState(float motorHealth)
{
    if(motorHealth < HEALTH_WARNING_LEVEL && motorHealth >= 0)
        return true;

    return false;
}


// ================= ALERT OUTPUT =================
void updateAlerts(String fault, bool warningState)
{

    if(fault == "STOPPED")
    {
        digitalWrite(LED_NORMAL, LOW);
        digitalWrite(LED_WARNING, LOW);
        digitalWrite(LED_FAULT, LOW);
        digitalWrite(BUZZER_PIN, LOW);

        normalLED = 0;
        warningLED = 0;
        faultLED = 0;
        buzzer = 0;
    }

    // ===== FAULT CONDITION (Highest Priority) =====
    else if(fault != "NORMAL")
    {
        digitalWrite(LED_NORMAL, LOW);
        digitalWrite(LED_WARNING, LOW);
        digitalWrite(LED_FAULT, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);

        normalLED = 0;
        warningLED = 0;
        faultLED = 1;
        buzzer = 1;
    }

    // ===== WARNING CONDITION =====
    else if(warningState)
    {
        digitalWrite(LED_NORMAL, LOW);
        digitalWrite(LED_WARNING, HIGH);
        digitalWrite(LED_FAULT, LOW);
        digitalWrite(BUZZER_PIN, LOW);

        normalLED = 0;
        warningLED = 1;
        faultLED = 0;
        buzzer = 0;
    }

    // ===== NORMAL CONDITION =====
    else
    {
        digitalWrite(LED_NORMAL, HIGH);
        digitalWrite(LED_WARNING, LOW);
        digitalWrite(LED_FAULT, LOW);
        digitalWrite(BUZZER_PIN, LOW);

        normalLED = 1;
        warningLED = 0;
        faultLED = 0;
        buzzer = 0;
    }
}

#endif