/*
------------------------------------------------------------
Project : MotorMind AI v1
Device  : Arduino UNO + I2C LCD (16x2)
Purpose : Display motor system status received from ESP32

Communication:
ESP32  ---->  Arduino
TX (GPIO17 from ESP32) ---> Pin 2 (SoftwareSerial RX)

Display:
Line 1 : Project name
Line 2 : Motor status / system message

Possible messages from ESP32:
STATUS:RUNNING
STATUS:STOPPED
FAULT:OVERHEAT
FAULT:UNBALANCE
FAULT:OVERCURRENT
WIFI:CONNECTED
WIFI:CONNECTING
BLYNK:ONLINE
BLYNK:OFFLINE
MAINT:ALERT

------------------------------------------------------------
*/

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <SoftwareSerial.h>


// Create LCD object (auto-detects I2C address and configuration)
hd44780_I2Cexp lcd;


// Software serial port for communication with ESP32
// Pin 2 = RX (receives data from ESP32)
// Pin 3 = TX (not used but required by SoftwareSerial)
SoftwareSerial espSerial(2,3);


// Variable that stores the latest status message from ESP32
String receivedMessage = "";


// Project title displayed on first line
String projectTitle = "MotorMind AI v1";


void setup()
{
  // Initialize hardware serial for debugging (Arduino Serial Monitor)
  Serial.begin(9600);

  // Initialize SoftwareSerial communication with ESP32
  espSerial.begin(9600);

  // Initialize LCD (16 columns, 2 rows)
  lcd.begin(16,2);

  // Turn on LCD backlight
  lcd.backlight();

  // Print project title on first line
  lcd.setCursor(0,0);
  lcd.print(projectTitle);
}


void loop()
{

/*
------------------------------------------------------------
1. RECEIVE STATUS MESSAGE FROM ESP32
------------------------------------------------------------
ESP32 sends text messages such as:
STATUS: RUNNING
FAULT: OVERHEAT
WIFI: CONNECTED

This section reads the message and stores it in statusText
*/

if (espSerial.available() > 0)
{
  receivedMessage = espSerial.readStringUntil('\n');
  receivedMessage.trim();

  // Print received message to Serial Monitor (for debugging)
  Serial.println(receivedMessage);
}


/*
------------------------------------------------------------
2. DISPLAY PROJECT NAME (LINE 1)
------------------------------------------------------------
The first line always shows the project title.
*/

lcd.setCursor(0,0);
lcd.print("MotorMind AI v1");


/*
------------------------------------------------------------
3. DISPLAY SYSTEM STATUS (LINE 2)
------------------------------------------------------------
Second line shows motor condition or system state.
Example:
STATUS: RUNNING
FAULT: OVERHEAT
WIFI: CONNECTED
*/

lcd.setCursor(0,1);

// Clear previous text
lcd.print("                ");

lcd.setCursor(0,1);

// Check message type and display accordingly

if(receivedMessage.startsWith("STATUS"))
{
  lcd.print(receivedMessage);
}

else if(receivedMessage.startsWith("FAULT"))
{
  lcd.print(receivedMessage);
}

else if(receivedMessage.startsWith("WIFI"))
{
  lcd.print(receivedMessage);
}

else if(receivedMessage.startsWith("BLYNK"))
{
  lcd.print(receivedMessage);
}

else if(receivedMessage.startsWith("MAINT"))
{
  lcd.print("MAINTENANCE!");
}

else
{
  lcd.print("SYSTEM READY");
}


// Small delay for stable LCD refresh
delay(300);

// Small delay for stable display refresh
delay(300);

}