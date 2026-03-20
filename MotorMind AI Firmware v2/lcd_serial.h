/*
=====================================================
LCD SERIAL COMMUNICATION MODULE
MotorMind AI v1

Handles:
  ESP32 → Arduino communication
  Sends status messages for LCD display
=====================================================
*/

#ifndef LCD_SERIAL_H
#define LCD_SERIAL_H

String lastLCDmsg = "";

// ================= SEND MESSAGE =================
void sendLCD(String msg)
{
    if(msg != lastLCDmsg) // To avoid flooding
    {
        Serial2.println(msg);
        lastLCDmsg = msg;
    }
}

#endif