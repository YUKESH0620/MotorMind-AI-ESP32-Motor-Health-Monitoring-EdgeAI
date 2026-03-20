/*
=============================================================
PROJECT : MotorMind AI v1
DEVICE  : 7Semi ESP32
AUTHOR  : Yukesh
PURPOSE : Real-time motor fault detection, IoT monitoring and early failure predicting
Edge-AI based motor health monitoring system.

FEATURES
-------------------------------------------------------------
  Vibration monitoring (ADXL345)
  Current monitoring (ACS712)
  Temperature monitoring (DS18B20)
  RPM measurement (IR sensor)
  Edge AI fault detection (Decision Tree)
  Predictive maintenance estimation
  OLED local data monitoring
  Arduino LCD status display
  IoT monitoring using Blynk
=============================================================
*/
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "motor_model.h"


Eloquent::ML::Port::DecisionTree model;

#define IR_PIN 27
#define CURRENT_PIN 34
#define ONE_WIRE_BUS 4
#define BUZZER_PIN 26
#define LED_NORMAL 18
#define LED_WARNING 19
#define LED_FAULT 33

#define LOG_DEBUG   1
#define LOG_INFO    2
#define LOG_DATASET 3

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL322mbuW0v"
#define BLYNK_TEMPLATE_NAME "Motor Health Monitoring"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = "VbxTljZqaT62lplxVUMRFzPgEYrbAH2H";
char ssid[] = "POCO C65";
char pass[] = "12202006";

int logMode = LOG_DEBUG;
String lastStatus = "";
String lastLCDmsg = "";
BlynkTimer timer;

// ================= SENSOR OBJECTS =================
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);


// ================= RPM VARIABLES =================
volatile unsigned long pulseCount = 0;
volatile unsigned long lastPulseTime = 0;

float filteredRPM = 0;
unsigned long lastMeasurementTime = 0;

// ================= FAULT DETECTION =================
int faultCounter = 0;
String motorStatus = "STOPPED";
String fault = "NORMAL";

// ================= THRESHOLDS =================
float vibrationThreshold = 0.35;
float currentThreshold = 0.25;
float temperatureThreshold = 60.0;
int abnormalCount = 0;


// ================= CURRENT SENSOR =================
float sensitivity = 0.1;
float offsetVoltage = 2.5;

// ================= DISPLAY VARIABLES =================
float vib_rms;
float current_rms;
float rpm;
float tempC;
bool faultLedState = false;
  unsigned long lastBlinkTime = 0;
float motorHealth = 100; //for Low pass filter for Health
float previousHealth = 100;
// Data for the Blynk app
int normalLED = 0;
int warningLED = 0;
int faultLED = 0;
int buzzer = 0;
float temperature = 0;
float vibration = 0;
float current = 0;
int motorhealth = 0;
int Rpm = 0;
int RUL = 100;
bool alertSent = false;
String currentStatus;
String maintenance;


// ================= RPM INTERRUPT =================
void IRAM_ATTR countPulse()
{
  unsigned long now = micros();

  if (now - lastPulseTime > 10000)
  {
    pulseCount++;
    lastPulseTime = now;
  }
}

/* 
=========================================================
   OLED DISPLAY FUNCTION
   Shows sensor data + motor health
========================================================= */
Adafruit_SSD1306 display(128, 64, &Wire, -1); // Initialization

void displayData()
{
  display.clearDisplay();
  display.setTextColor(WHITE);

  if(fault == "STOPPED")
  {
  display.setTextSize(2);
  display.setCursor(10,20);
  display.println("STOPPED");
  sendLCD("STATUS:STOPPED");

  digitalWrite(BUZZER_PIN, LOW);   // buzzer OFF
  }
  else {
  display.setTextSize(1);

    display.setCursor(0,0);
    display.println("Motor Data Monitor");

    display.setCursor(0,12);
    display.print("VIB: ");
    display.println(vib_rms);

    display.setCursor(0,22);
    display.print("CUR: ");
    display.println(current_rms);

    display.setCursor(0,32);
    display.print("RPM: ");
    display.println((int)rpm);

    display.setCursor(0,42);
    display.print("TEMP: ");
    display.print(tempC);
    display.println("`C");

    display.setCursor(80,12);
    display.print("HLT:");
    display.print((int)motorHealth);
    display.println("%");
    int barWidth = map(motorHealth, 0, 100, 0, 40);  
    barWidth = constrain(barWidth, 0, 40);
    display.fillRect(80,20,40,6,BLACK);
    display.drawRect(80, 20, 40, 6, WHITE);   // outline of bar
    display.fillRect(80, 20, barWidth, 6, WHITE); // filled health level
  
    if(fault == "NORMAL")
    {
      display.setCursor(0,54);
      display.println("STATUS: NORMAL");

      digitalWrite(BUZZER_PIN, LOW); // Buzzer OFF at Normal Condition
      
    }

    else
    {

  display.setTextSize(1);
  display.setCursor(0,54);
  display.print("STATUS: ");
  display.println(fault);
  
  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(LED_NORMAL, LOW);
  digitalWrite(LED_WARNING, LOW);
  digitalWrite(LED_FAULT, HIGH);
    // buzzer ON at fault condition
  
}
}

  display.display();   // show OLED content
}
void sendLCD(String msg)
{
  if(msg != lastLCDmsg)
  {
    Serial2.println(msg);
    lastLCDmsg = msg;
  }
}

void sendDataToBlynk() {   // Send Variables to Blynk
    
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

// ================= SETUP =============================================
void setup()
{ 
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, -1, 25); // Arduino communication

  Wire.begin(21,22);

  sendLCD("WIFI:CONNECTING");

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  sendLCD("WIFI:CONNECTED");
  sendLCD("Blynk:OFFLINE");
  Blynk.begin(auth, ssid, pass);
  sendLCD("Blynk:ONLINE");
  timer.setInterval(2000L, sendDataToBlynk);
  
  pinMode(LED_NORMAL, OUTPUT);
  pinMode(LED_WARNING, OUTPUT);
  pinMode(LED_FAULT, OUTPUT);

  digitalWrite(LED_NORMAL, LOW);
  digitalWrite(LED_WARNING, LOW);
  digitalWrite(LED_FAULT, LOW);

  if(!accel.begin())
  {
     Serial.println("ADXL345 not detected");
     while(1);
  }

  accel.setRange(ADXL345_RANGE_2_G);

  tempSensor.begin();

  pinMode(IR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(IR_PIN), countPulse, RISING);

  analogReadResolution(12);
  analogSetPinAttenuation(CURRENT_PIN, ADC_11db);

  // ===== CURRENT SENSOR CALIBRATION =====
  float sum = 0;

  for (int i = 0; i < 500; i++)
  {
    sum += (analogRead(CURRENT_PIN) * 3.3) / 4095.0;
    delay(2);
  }

  offsetVoltage = sum / 500.0;
  // ======= OLED AND BUZZER ==================
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // ===== STARTUP SCREEN =====
  display.clearDisplay();
  display.setTextColor(WHITE);

  // Title_OLED
  display.setTextSize(1);
  display.setCursor(0,10);
  display.println("Edge AI Based");

  // Subtitle_OLED
  display.setCursor(0,18);
  display.println("Motor Health");

  display.setTextSize(1);
  display.setCursor(0,28);
  display.println("Monitoring");
  display.setCursor(0,38);
  display.println("System - v1.26");
  display.setCursor(0,48);
  display.println("Starting...");
  display.display();


  delay(2500);
    
}
// ================= MAIN LOOP ==================
void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    Blynk.run();
  }
  timer.run();

  if (millis() - lastMeasurementTime >= 1000)
  {
    unsigned long currentTime = millis();
    lastMeasurementTime = currentTime;

    // ================= RPM =====================
    noInterrupts();
    unsigned long count = pulseCount;
    pulseCount = 0;
    interrupts();

    float rawRPM = count * 60.0;

    if(rawRPM < 100 || rawRPM > 3000)
      rawRPM = 0;

    filteredRPM = 0.7 * filteredRPM + 0.3 * rawRPM;

    // ================= VIBRATION =================
    int vibSamples = 150;

    float vibSum = 0;
    float vibSumSquares = 0;

    for(int i=0;i<vibSamples;i++)
    {
      sensors_event_t event;
      accel.getEvent(&event);

      float magnitude =
        sqrt(event.acceleration.x * event.acceleration.x +
             event.acceleration.y * event.acceleration.y +
             event.acceleration.z * event.acceleration.z);

      vibSum += magnitude;
      vibSumSquares += magnitude * magnitude;
    }

    float vibMean = vibSum / vibSamples;
    float vibRMS = sqrt(vibSumSquares / vibSamples);
    float vibStdDev = sqrt((vibSumSquares / vibSamples) - (vibMean * vibMean));

    // ================= CURRENT =================
    int currentSamples = 500;
    int validSamples = 0;

    float currentSum = 0;
    float currentSumSquares = 0;

    for(int i=0;i<currentSamples;i++)
    {
      int adcValue = analogRead(CURRENT_PIN);

      if(adcValue > 4000 || adcValue < 10)
        continue;

      validSamples++;

      float voltage = (adcValue * 3.3) / 4095.0;
      float current = (voltage - offsetVoltage) / sensitivity;

      if(abs(current) < 0.05)
        current = 0;

      currentSum += abs(current);
      currentSumSquares += current * current;
    }

    float currentMean = currentSum / validSamples;
    float currentRMS = sqrt(currentSumSquares / validSamples);
    float currentStdDev = sqrt((currentSumSquares / validSamples) - (currentMean * currentMean));

    // ================= TEMPERATURE =================
    tempSensor.requestTemperatures();
    tempC = tempSensor.getTempCByIndex(0);

 // ===== MOTOR HEALTH SCORE =====

    float vibRatio = vibStdDev / vibrationThreshold;
float currentRatio = currentRMS / currentThreshold;
float tempRatio = tempC / temperatureThreshold;

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
    motorHealth = 0.3 * rawHealth + 0.7 * previousHealth; // Low pass filter
    previousHealth = motorHealth;

   if(motorHealth > 100) motorHealth = 100; // Limit health range
   if(motorHealth < 0) motorHealth = 0;

   RUL = motorHealth * 1.2;
  
   if (RUL > 80)
    {
        maintenance = "Healthy";
    }
   else if (RUL > 50)
    {
        maintenance = "Service Soon";
    }
   else
    {
       maintenance = "Immediate Maintenance Required";
    }

  // ===== AI FEATURE ARRAY ==========================
   float features[8];

   features[0] = vibRMS;
   features[1] = vibMean;
   features[2] = vibStdDev;
   features[3] = currentRMS;
   features[4] = currentMean;
   features[5] = currentStdDev;
   features[6] = filteredRPM;
   features[7] = tempC;


// =============== AI PREDICTION LOGIC ===================
   int aiPrediction = model.predict(features);


  fault = "NORMAL";


if(filteredRPM < 100)
{
  fault = "STOPPED";
  abnormalCount = 0;
}

if(filteredRPM > 250)
{
  bool abnormal = false;
  String detectedFault = "NORMAL";


      if(aiPrediction == 4)
      {
         abnormal = true;
         detectedFault = "UNBALANCE";
      }
      else if(aiPrediction == 1)
      {
         abnormal = true;
         detectedFault = "OVERCURRENT";
      }
      else if(aiPrediction == 2)
      {
         abnormal = true;
         detectedFault = "OVERHEAT";
      }

  // Fault confirmation logic
  if(abnormal)
  {
     abnormalCount++;
  }
  else
  {
     abnormalCount = 0;
  }

  // Trigger fault only after 3 abnormal cycles
  if(abnormalCount >= 3)
  {
     fault = detectedFault;
  }
  else
  {
     fault = "NORMAL";
  }
// Fault state sent to LCD 
  if(fault == "UNBALANCE")
  {
    sendLCD("FAULT:UNBALANCE");
  }
  else if(fault == "OVERCURRENT")
  {
     sendLCD("FLT:OVERCURRENT");
  }
  else if(fault == "OVERHEAT")
  {
    sendLCD("FAULT:OVERHEAT");
  }
  else
  {
    sendLCD("STATUS:NORMAL");
  }
}
    
    
   bool warningState = false;

   if(motorHealth < 75 && motorHealth >= 0)
   {
       warningState = true;
   }
    
    // ================= LED STATUS =================
    if(fault == "STOPPED" )
    {
      digitalWrite(LED_NORMAL, LOW);
      digitalWrite(LED_WARNING, LOW);
      digitalWrite(LED_FAULT, LOW);
      warningState = false;
    }

    else if(fault == "NORMAL" && !warningState)
    {
       digitalWrite(LED_NORMAL, HIGH);
       digitalWrite(LED_WARNING, LOW);
       digitalWrite(LED_FAULT, LOW);
       normalLED = 1;
       warningLED = 0;
       faultLED = 0;
       buzzer = 0;
    }

    else if(warningState)
    {
       digitalWrite(LED_NORMAL, LOW);
       digitalWrite(LED_WARNING, HIGH);
       digitalWrite(LED_FAULT, LOW);
       normalLED = 0;
       warningLED = 1;
       faultLED = 0;
       buzzer = 0;
    }
 
    else
    {
      digitalWrite(LED_NORMAL, LOW);
      digitalWrite(LED_WARNING, LOW);
      digitalWrite(LED_FAULT, HIGH);
      normalLED = 0;
      warningLED = 0;
      faultLED = 1;
      buzzer = 1;
    }

    
  
    // ================= ASSIGN OLED VALUES =================
    vib_rms = vibRMS;
    current_rms = currentRMS;
    rpm = filteredRPM;

    // ================= OLED =================
    displayData();
    // ==============Blynk=====================
    temperature = tempC;     // from temperature sensor
    vibration = vibRMS;      // vibration RMS value
    current = currentRMS;  // ACS712 value
    Rpm = filteredRPM;          // calculated speed
    motorhealth = motorHealth;    // calculated health index

    // SMS through Blynk for alert
    if (motorHealth < 30 && !alertSent)
      {
        Blynk.logEvent("motor_fault", "Motor health critical! Maintenance required.");
        Serial2.println("MAINT:ALERT");
        alertSent = true;
      }

    if (motorHealth > 40)
      {
        alertSent = false;
      }
 
    if(Serial.available())
{
   char cmd = Serial.read();

   if(cmd == 'd')
   {
      logMode = LOG_DEBUG;
      Serial.println("Switched to DEBUG mode");
   }
   else if(cmd == 'i')
   {
      logMode = LOG_INFO;
      Serial.println("Switched to INFO mode");
   }
   else if(cmd == 'c')
   {
      logMode = LOG_DATASET;
      Serial.println("Switched to DATASET mode");
   }
}
    
    // ================= SERIAL OUTPUT =================
    if(logMode == LOG_DEBUG)
{
   Serial.println("\n====== DEBUG ======");
   Serial.print("VibRMS: "); Serial.println(vibRMS); 
   Serial.print("Vibmean: "); Serial.println(vibMean);
   Serial.print("VibStdDev: "); Serial.println(vibStdDev); 
   Serial.print("CurrentRMS: "); Serial.println(currentRMS); 
   Serial.print("Current Mean: "); Serial.println(currentMean); 
   Serial.print("CurrentStdDev: "); Serial.println(currentStdDev);
   Serial.print("RPM: "); Serial.println(filteredRPM);
   Serial.print("Temp: "); Serial.println(tempC);
   Serial.print("AI Class: "); Serial.println(aiPrediction);
   Serial.print("Status: "); Serial.println(fault);
   Serial.print("LCD_Status: "); Serial.println(currentStatus);
   Serial.print("Health: "); Serial.print(motorHealth); Serial.println("%");
   Serial.println("===================");
}

else if(logMode == LOG_INFO)
{
   Serial.print("RPM:");
   Serial.print(filteredRPM);
   Serial.print(" | Temp:");
   Serial.print(tempC);
   Serial.print(" | Health:");
   Serial.print(motorHealth);
   Serial.print("%");
   Serial.print(" | Status:");
   Serial.println(fault);
}

else if(logMode == LOG_DATASET)
{
   Serial.print(millis()); Serial.print(",");
   Serial.print(features[0]); Serial.print(",");
   Serial.print(features[1]); Serial.print(",");
   Serial.print(features[2]); Serial.print(",");
   Serial.print(features[3]); Serial.print(",");
   Serial.print(features[4]); Serial.print(",");
   Serial.print(features[5]); Serial.print(",");
   Serial.print(features[6]); Serial.print(",");
   Serial.print(features[7]); Serial.print(",");
   Serial.print(aiPrediction); Serial.print(",");
   Serial.println(fault);
}
  }
}
