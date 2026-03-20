/*
=====================================================
ACS712 SENSOR CALIBRATION MODULE
MotorMind AI v1

Purpose
-----------------------------------------------------
This module provides sensor calibration functions
used to improve measurement accuracy.

Calibration ensures that sensors start from a
correct baseline before normal operation.

Currently implemented:
  ACS712 Current Sensor Offset Calibration

Why Calibration is Needed
-----------------------------------------------------
Some sensors do not output exact zero values when
there is no signal. For example:

ACS712:
When no current flows, the output voltage should
ideally be 2.5V (for a 5V supply system). However,
due to manufacturing tolerances, the actual value
may slightly differ (e.g., 2.48V or 2.53V).

If this offset is not corrected, the system may
measure a small current even when the motor is OFF.

The calibration routine measures this offset and
stores it in a variable so that all future current
calculations subtract this error.

Important Notes
-----------------------------------------------------
  Calibration should be done when the motor is OFF.
  No load should be connected during calibration.
  This module is optional if sensor readings are
  already accurate.

Future Expansion
-----------------------------------------------------
This module can also include:
  ADXL345 vibration baseline calibration
  Temperature sensor offset correction
  RPM sensor timing calibration
=====================================================
*/

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "config.h"

// Global offset variable used in current calculation
extern float offsetVoltage;


/*
=====================================================
FUNCTION: calibrateCurrentSensor()

Purpose:
Measures the zero-current output voltage of the
ACS712 sensor and calculates the offset voltage.

How it works:
1. Reads ADC value from current sensor multiple times
2. Converts ADC value to voltage
3. Computes the average voltage
4. Stores the result as the offset voltage

This offset is later used in the current formula:

Current = (MeasuredVoltage - OffsetVoltage) / Sensitivity

This removes false current readings.
=====================================================
*/

void calibrateCurrentSensor()
{
    Serial.println("Calibrating Current Sensor...");

    float sum = 0;

    for(int i = 0; i < 500; i++)
    {
        float voltage = (analogRead(CURRENT_PIN) * 3.3) / 4095.0;
        sum += voltage;

        delay(2);
    }

    offsetVoltage = sum / 500.0;

    Serial.print("Current Offset Voltage: ");
    Serial.println(offsetVoltage);
}

#endif