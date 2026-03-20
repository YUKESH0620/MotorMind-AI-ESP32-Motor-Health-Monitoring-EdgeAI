/*
=====================================================
MOTOR HEALTH MODEL
MotorMind AI v1

Handles:
  Motor health calculation
  Remaining Useful Life estimation
  Maintenance recommendation
=====================================================
*/

#ifndef HEALTH_MODEL_H
#define HEALTH_MODEL_H

#include "config.h"

extern float previousHealth;


// ================= MOTOR HEALTH CALCULATION =================
float computeMotorHealth(
    float vibStdDev,
    float currentRMS,
    float temperature)
{

    float vibRatio = vibStdDev / VIBRATION_THRESHOLD;
    float currentRatio = currentRMS / CURRENT_THRESHOLD;
    float tempRatio = temperature / TEMPERATURE_THRESHOLD;

    float vibStress = 0;
    float currentStress = 0;
    float tempStress = 0;

    if(vibRatio > 0.6)
        vibStress = (vibRatio - 0.6) * 50;

    if(currentRatio > 0.6)
        currentStress = (currentRatio - 0.6) * 30;

    if(tempRatio > 0.7)
        tempStress = (tempRatio - 0.7) * 20;

    float rawHealth = 100 - (vibStress + currentStress + tempStress);

    float filteredHealth = 0.3 * rawHealth + 0.7 * previousHealth;

    previousHealth = filteredHealth;

    if(filteredHealth > 100) filteredHealth = 100;
    if(filteredHealth < 0) filteredHealth = 0;

    return filteredHealth;
}


// ================= REMAINING USEFUL LIFE =================
int estimateRUL(float motorHealth)
{
    int RUL = motorHealth * 1.2;

    if(RUL > 100)
        RUL = 100;

    return RUL;
}


// ================= MAINTENANCE MESSAGE =================
String maintenanceAdvice(int RUL)
{
    if (RUL > 80)
        return "Healthy";

    else if (RUL > 50)
        return "Service Soon";

    else
        return "Immediate Maintenance Required";
}

#endif