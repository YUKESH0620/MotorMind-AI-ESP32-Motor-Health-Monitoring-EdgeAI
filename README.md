# MotorMind AI v1.0

**Edge AI-Based Real-Time Motor Health Monitoring and Predictive Fault Detection Using ESP32**

---

## 1. Overview

MotorMind AI is an embedded Edge AI system designed for real-time monitoring and predictive fault detection of electric motors. The system integrates multi-sensor data acquisition with on-device machine learning to detect faults such as mechanical imbalance, overheating, and overcurrent conditions.

Unlike conventional monitoring systems that rely on threshold-based alerts or cloud processing, this system performs all computations locally on the ESP32 microcontroller, ensuring low latency, reliability, and independence from network connectivity.

The project demonstrates a complete pipeline from sensor acquisition to intelligent decision-making and user-level output.

---

## 2. Key Features

* Real-time monitoring of motor parameters (vibration, temperature, current, RPM)
* Multi-sensor data fusion for improved fault detection accuracy
* Feature extraction using statistical methods (RMS, mean, standard deviation)
* Embedded machine learning using a Decision Tree classifier
* On-device inference using Edge AI (no cloud dependency)
* Motor Health Index computation and Remaining Useful Life (RUL) estimation
* Multi-level output system (OLED, LCD, LEDs, buzzer)
* IoT-based remote monitoring via Blynk

---

## 3. System Architecture

The system is structured into three main layers:

### 3.1 Sensor Layer

* ADXL345 accelerometer for vibration analysis
* ACS712 current sensor for electrical monitoring
* DS18B20 temperature sensor for thermal analysis
* IR sensor for RPM measurement

### 3.2 Processing Layer

* ESP32 microcontroller
* Data acquisition and signal processing
* Feature extraction and normalization
* Embedded machine learning inference

### 3.3 Output Layer

* OLED display for real-time visualization
* LCD module (via Arduino UNO) for system status
* LED indicators for condition states
* Buzzer for fault alerts
* IoT dashboard for remote monitoring
The architecture enables complete on-device processing, reducing latency and eliminating reliance on cloud-based systems .

### 3.4 Flow Chart
<img width="991" height="726" alt="Flow_Chart_MotorMind" src="https://github.com/user-attachments/assets/3204f545-b034-41ad-ada1-8a1ae4ae17cd" />

The following flowchart illustrates the complete execution pipeline of the system, including sensor data acquisition, signal processing, feature extraction, AI-based fault detection, and output generation.

---

## 4. Hardware Components

The system consists of the following hardware components:

| S.No | Component            | Quantity | Description |
|------|---------------------|----------|------------|
| 1    | ESP32               | 1        | Main microcontroller for data processing and Edge AI inference |
| 2    | ADXL345             | 1        | 3-axis accelerometer for vibration analysis |
| 3    | ACS712              | 1        | Hall-effect current sensor for electrical monitoring |
| 4    | DS18B20             | 1        | Digital temperature sensor for thermal monitoring |
| 5    | IR Sensor           | 1        | Used for RPM measurement via pulse detection |
| 6    | OLED Display        | 1        | Displays real-time sensor data |
| 7    | Arduino UNO         | 1        | Used for controlling LCD display via UART |
| 8    | LCD Module          | 1        | Displays system status messages |
| 9    | LEDs                | 3        | Indicate Normal, Warning, and Fault conditions |
| 10   | Buzzer              | 1        | Provides audible alert for critical faults |
| 11   | Breadboard          | 1        | Prototyping platform |
| 12   | Jumper Wires        | Multiple | Electrical connections between components |
| 13   | Power Supply        | 2        | USB (ESP32) and battery (Arduino) |
| 14   | Capacitors          | Few      | Used for decoupling and noise filtering |

---

## 5. Pin Configuration

| Component                  | ESP32 Pin                | Function             |
| -------------------------- | ------------------------ | -------------------- |
| IR Sensor                  | GPIO27                   | RPM measurement      |
| ACS712 Current Sensor      | GPIO34                   | Analog input         |
| DS18B20 Temperature        | GPIO4                    | One-wire interface   |
| OLED SDA / SCL             | GPIO21 / GPIO22          | I2C communication    |
| ADXL345 SDA / SCL          | GPIO21 / GPIO22          | Shared I2C           |
| Buzzer                     | GPIO26                   | Alert output         |
| LED (Normal/Warning/Fault) | GPIO18 / GPIO19 / GPIO33 | Status indication    |
| UART TX (to Arduino)       | GPIO25                   | Serial communication |

---

## 6. Software Design

The system software is implemented using embedded C/C++ on the ESP32 platform. It follows a continuous execution loop:

1. Sensor initialization and data acquisition
2. Signal conditioning and noise reduction
3. Feature extraction from vibration and current signals
4. AI-based classification of motor condition
5. Motor health computation and RUL estimation
6. Output display and IoT data transmission

This structured pipeline ensures efficient execution of real-time monitoring tasks .

---

## 7. Edge AI Implementation

* Model Type: Decision Tree Classifier
* Deployment: Embedded directly on ESP32
* Input Features:

  * Vibration RMS, Mean, Standard Deviation
  * Current RMS, Mean, Standard Deviation
  * Temperature
  * RPM

The model classifies motor conditions into:

* Normal
* Warning
* Fault (Overcurrent, Overheating, Imbalance)

A confirmation mechanism is implemented to avoid false positives by validating abnormal conditions over multiple cycles .

---

## 8. Motor Health Evaluation

The system computes a Motor Health Index based on:

* Vibration irregularities
* Current deviations
* Temperature rise

Using this index:

* Remaining Useful Life (RUL) is estimated
* Maintenance recommendations are generated

This enables predictive maintenance rather than reactive fault handling.

---

## 9. Results

The system was tested under multiple operating conditions. Observations include:

* Accurate detection of electrical and mechanical faults
* Stable real-time data acquisition and processing
* Reliable classification of motor states (normal, warning, fault)
* Effective multi-sensor correlation for improved decision accuracy

The results validate the effectiveness of Edge AI-based monitoring for industrial applications.

---

## 10. Output and Monitoring

The system provides multiple output channels:

* OLED display for live parameter visualization
* LCD module for system-level status
* LEDs indicating operational states
* Buzzer for critical alerts
* IoT dashboard for remote monitoring and notifications

---

## 11. Project Media

### 11.1 Full System Setup

![20260319_011255](https://github.com/user-attachments/assets/094b40ae-7b13-4905-9de2-0717695745e6)


### 11.2 Hardware Prototype

![20260319_003133](https://github.com/user-attachments/assets/e65a565a-1b4c-45c5-b31e-f223d6ebbf5c)


### 11.3 IoT Dashboard
<img width="720" height="1600" alt="image" src="https://github.com/user-attachments/assets/b04729cc-2073-4167-972a-82be5d728a08" />

<img width="720" height="1600" alt="image" src="https://github.com/user-attachments/assets/f879d29b-0145-4c88-a74a-c2096c36099f" />


### 11.4 Demonstration Video

https://youtu.be/cJwQofUENSA

---

## 12. Project Structure

```
MotorMind-AI/
│
├── firmware/
├── dataset/
├── hardware/
├── images/
├── report/
└── README.md
```

---

## 13. Documentation

Full technical documentation is available in:

`report/MotorMind_v1.0_Report.pdf`

---

## 14. Limitations

* Breadboard-based implementation may introduce connection instability
* Limited advanced signal processing (FFT not fully implemented)
* Power synchronization required between ESP32 and Arduino

---

## 15. Future Work

* Implementation of advanced ML models (Neural Networks)
* FFT-based vibration analysis for deeper fault detection
* PCB design for improved hardware reliability
* Mobile application for monitoring and control
* Integration with cloud-based analytics platforms

---

## 16. Author

Yukesh.S
B.E Electrical and Electronics Engineering
College of Engineering, Guindy (Anna University)

---

## 17. Copyright

© 2026 Yukesh. All rights reserved.

This project is provided for academic and demonstration purposes only. Unauthorized reproduction, distribution, or modification is not permitted.

---

