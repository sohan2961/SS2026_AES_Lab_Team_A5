# Smart Greenhouse System with Fire Detection

## Project Overview

The **Smart Greenhouse System with Fire Detection** is an IoT-based embedded system project designed to monitor greenhouse environmental conditions and provide automatic safety responses. The system uses a **Raspberry Pi 5** as the central controller and dashboard, while **ESP32 microcontrollers** collect sensor data and control actuators.

The main goal of this project is to improve plant monitoring, automate irrigation, and detect dangerous conditions such as fire, smoke, gas leakage, and poor air quality.

## Block Diagram

![Smart Greenhouse System Block Diagram](smart_greenhouse_block_diagram.png)

## Hardware Elements

The following hardware components are used in this project:

1. Raspberry Pi 5  
2. 3 ESP32 microcontrollers  
3. MQ-2 smoke/gas sensor  
4. IR flame sensor  
5. Relay module and water pump  
6. I2C LCD display  
7. LEDs  
8. Resistors  
9. Jumper wires and breadboard  
10. Buzzer  
11. Soil moisture sensor  
12. Temperature sensors  
13. MQ-135 air quality sensor  

## System Architecture

The system is divided into one central controller and three ESP32-based nodes.

### Raspberry Pi 5

The Raspberry Pi 5 works as the main controller and monitoring unit. It can be used for:

- Running the MQTT broker
- Receiving data from ESP32 nodes
- Storing sensor data
- Displaying values on a dashboard
- Making control decisions
- Sending commands to ESP32 actuator nodes

### ESP32-1: Environment Monitoring Node

This ESP32 is responsible for monitoring the greenhouse environment.

Connected components:

- Soil moisture sensor
- Temperature sensor
- I2C LCD display

Main functions:

- Read soil moisture value
- Read temperature value
- Display local values on the LCD
- Send sensor data to the Raspberry Pi through Wi-Fi/MQTT

### ESP32-2: Fire and Gas Safety Node

This ESP32 is used for safety monitoring.

Connected components:

- MQ-2 smoke/gas sensor
- IR flame sensor
- MQ-135 air quality sensor
- Buzzer
- Warning LED

Main functions:

- Detect smoke or gas leakage
- Detect flame/fire
- Monitor air quality
- Activate buzzer during emergency
- Turn on warning LED
- Send alert messages to the Raspberry Pi

### ESP32-3: Actuator Control Node

This ESP32 controls the output devices.

Connected components:

- Relay module
- Water pump
- Status LEDs

Main functions:

- Receive control commands from the Raspberry Pi
- Turn the water pump ON or OFF
- Control status LEDs
- Support automatic irrigation

## Communication Method

The system uses **Wi-Fi and MQTT** for communication between the Raspberry Pi 5 and ESP32 boards.

Example MQTT topics:

```text
greenhouse/soil
greenhouse/temperature
greenhouse/smoke
greenhouse/flame
greenhouse/airquality
greenhouse/pump
greenhouse/alarm
```

## Working Principle

1. Sensors collect real-time data from the greenhouse.
2. ESP32 nodes send sensor data to the Raspberry Pi 5 using MQTT.
3. The Raspberry Pi processes the received data.
4. If soil moisture is low, the water pump can be turned ON automatically.
5. If fire, smoke, or gas is detected, the buzzer and warning LED are activated.
6. Sensor values and system status can be displayed on a local LCD and web dashboard.

## Main Features

- Real-time greenhouse monitoring
- Automatic irrigation control
- Fire and smoke detection
- Gas and air quality monitoring
- Local LCD display
- Warning system using buzzer and LEDs
- Wireless communication using ESP32 and MQTT
- Raspberry Pi-based dashboard and data logging

## Possible Future Improvements

- Add humidity sensor
- Add camera monitoring
- Add mobile app notification
- Store sensor data in a database
- Add solar power support
- Add remote control through a web dashboard

## Conclusion

This project combines environmental monitoring, IoT communication, automatic irrigation, and fire detection in one smart greenhouse system. It is suitable for an embedded systems or IoT-based lab project because it includes sensors, actuators, wireless communication, real-time monitoring, and safety features.
