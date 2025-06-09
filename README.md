# Waiter Robot – ESP32  + BLDC Control

This project is a mobile "Waiter Robot" platform developed to autonomously navigate indoor spaces and deliver items. It was created as part of the **"Design of Mechatronic Systems"** course at the **Wrocław University of Science and Technology**.

The robot combines the computing power of a **Raspberry Pi** with the real-time responsiveness of an **ESP32** microcontroller. It is powered by **BLDC motors** from a hoverboard and is equipped with **ultrasonic sensors** for obstacle detection and simple collision avoidance.

## 🧠 System Architecture

- **Raspberry Pi (High-Level Controller)**  
  - Detects **ArUco markers on the ceiling** to determine the robot's position and orientation  
  - Calculates target movement directions  
  - Sends navigation commands to the ESP32 via **UART**

- **ESP32 (Low-Level Controller)**  
  - Receives motion commands from Raspberry Pi  
  - Controls two BLDC motors using a **hoverboard motor driver**  
  - Handles **obstacle detection** using HC-SR04 ultrasonic sensors  
  - Can optionally receive **manual control input via I-BUS** from a radio controller

## 🔧 Hardware Components

- **Microcontrollers**:  
  - ESP32 (motor + sensor control, UART & I-BUS interface)  
  - Raspberry Pi (vision & navigation logic)

- **Motors**:  
  - 2 × BLDC motors (from a hoverboard)  
  - Hoverboard motor controller (custom firmware or stock driver)

- **Sensors**:  
  - 3 × HC-SR04 ultrasonic sensors for obstacle detection  
  - Ceiling-mounted ArUco markers for visual localization

- **Manual Control Option**:  
  - Radio controller with **I-BUS** protocol support

## ✅ Features

- 📷 Visual localization using ArUco markers
- 🔁 UART communication between Raspberry Pi and ESP32
- 🛞 BLDC motor control via hoverboard driver
- 👀 Obstacle detection with three ultrasonic sensors
- 🎮 Optional manual control via I-BUS RC controller
---

🤖 *Built as a course project and proof-of-concept — possibly to be expanded in future iterations!*
