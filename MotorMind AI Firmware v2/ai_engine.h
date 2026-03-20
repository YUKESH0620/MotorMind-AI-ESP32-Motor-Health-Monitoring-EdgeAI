/*
=====================================================
AI ENGINE MODULE
MotorMind AI v1

Handles:
  Decision Tree inference
  Fault classification
  Abnormal cycle confirmation
=====================================================
*/

#ifndef AI_ENGINE_H
#define AI_ENGINE_H

#include "config.h"
#include "motor_model.h"

extern Eloquent::ML::Port::DecisionTree model;

extern int abnormalCount;
extern String fault;


// ================= RUN AI INFERENCE =================
int runAI(float features[8])
{
    return model.predict(features);
}


// ================= FAULT CLASSIFICATION =================
String classifyFault(int aiPrediction)
{
    switch(aiPrediction)
    {
        case 4:
            return "UNBALANCE";

        case 1:
            return "OVERCURRENT";

        case 2:
            return "OVERHEAT";

        default:
            return "NORMAL";
    }
}


// ================= FAULT CONFIRMATION =================
String updateFaultState(int aiPrediction, float rpm)
{
    bool abnormal = false;
    String detectedFault = "NORMAL";

    if(rpm < RPM_MIN_VALID)
    {
        abnormalCount = 0;
        return "STOPPED";
    }

    if(rpm > 250)
    {
        detectedFault = classifyFault(aiPrediction);

        if(detectedFault != "NORMAL")
            abnormal = true;
    }

    // abnormal cycle counting
    if(abnormal)
        abnormalCount++;
    else
        abnormalCount = 0;

    // confirm fault only after multiple detections
    if(abnormalCount >= ABNORMAL_CONFIRM_COUNT)
    {
        return detectedFault;
    }
    return "NORMAL";
}

#endif