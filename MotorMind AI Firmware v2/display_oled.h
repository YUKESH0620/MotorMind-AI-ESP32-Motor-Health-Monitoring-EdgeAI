/*
=====================================================
OLED DISPLAY MODULE
MotorMind AI v1

Handles:
  OLED initialization
  Motor data display
  Health bar visualization
  Fault display
=====================================================
*/

#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "syk_oled_bitmap.h"

// OLED object
extern Adafruit_SSD1306 display;

// ================= STARTUP SCREEN =================
void showStartupScreen()
{   
    // Show SYK Logo
    display.clearDisplay();
    display.drawBitmap(0, 0, syk_logo, 128, 64, WHITE);
    display.display();
    delay(3000);   // show logo for 3 seconds

    // Show Project Information
    display.clearDisplay();
    display.setTextColor(WHITE);

    display.setTextSize(1);
    display.setCursor(24,10);
    display.println("Edge AI Based");

    display.setCursor(22,20);
    display.println("Motor Health");

    display.setCursor(28,30);
    display.println("Monitoring");

    display.setCursor(26,40);
    display.println("System - v1.26");

    display.setCursor(32,52);
    display.println("Starting...");

    display.display();

    delay(2500);
}


// ================= STOPPED SCREEN =================
void showStoppedScreen()
{
    display.clearDisplay();
    display.setTextColor(WHITE);

    display.setTextSize(2);
    display.setCursor(10,20);
    display.println("STOPPED");

    display.display();
}


// ================= HEALTH BAR =================
void drawHealthBar(int motorHealth)
{
    int barWidth = map(motorHealth, 0, 100, 0, 40);
    barWidth = constrain(barWidth, 0, 40);

    display.fillRect(80,20,40,6,BLACK);
    display.drawRect(80,20,40,6,WHITE);
    display.fillRect(80,20,barWidth,6,WHITE);
}


// ================= MAIN DATA DISPLAY =================
void displayMotorData(
        float vib_rms,
        float current_rms,
        float rpm,
        float tempC,
        int motorHealth,
        String fault)
{

    display.clearDisplay();
    display.setTextColor(WHITE);
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
    display.println("C");

    display.setCursor(80,12);
    display.print("HLT:");
    display.print(motorHealth);
    display.println("%");

    drawHealthBar(motorHealth);

    display.setCursor(0,54);
    display.print("STATUS: ");
    display.println(fault);

    display.display();
}

#endif