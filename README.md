# Smart Greenhouse Monitoring System with Fire and Gas Detection

This repository is divided into **one main project folder** and **four member folders**. Each member is responsible for one node of the total IoT greenhouse system.

## Project Dashboard

> This dashboard is automatically updated by GitHub Actions after every commit.

<!-- AUTO_DASHBOARD_START -->
| Member | Responsible Node | Folder | Commits | Last Activity | Progress |
|---|---|---|---:|---|---|
| Md Mostafizur Rahman | Raspberry Pi 3 — Central Controller | `01_RaspberryPi_Central_Controller_MdMostafizurRahman/` | 3 | 2026-06-04 by Md Mostafizur Rahman | 60% |
| Turja Barua | ESP32-1 — Environment Monitoring Node | `02_ESP32_Environment_Node_TurjaBarua/` | 3 | 2026-06-04 by Md Mostafizur Rahman | 60% |
| Moaj Chowdhury | ESP32-2 — Fire and Gas Safety Node | `03_ESP32_FireGas_Safety_Node_MoajChowdhury/` | 3 | 2026-06-04 by Md Mostafizur Rahman | 60% |
| Deepak Kapil | Arduino Uno WiFi Rev2 — Actuator / Relay Node | `04_Arduino_Relay_Node_DeepakKapil/` | 3 | 2026-06-04 by Md Mostafizur Rahman | 60% |
<!-- AUTO_DASHBOARD_END -->

## Team Task Division

### 1. Md Mostafizur Rahman — Raspberry Pi 3 Central Controller
Folder: `01_RaspberryPi_Central_Controller_MdMostafizurRahman/`

Main tasks:
- Install and configure MQTT broker on Raspberry Pi 3.
- Create Python monitoring program.
- Receive MQTT data from ESP32 and Arduino nodes.
- Print or store sensor data.
- Show dashboard or terminal status.
- Send actuator commands to Arduino relay node.

Expected output:
- MQTT broker running on Raspberry Pi.
- Python dashboard or terminal monitor.
- Logs of temperature, humidity, gas, flame, and relay status.

### 2. Turja Barua — ESP32-1 Environment Monitoring Node
Folder: `02_ESP32_Environment_Node_TurjaBarua/`

Main tasks:
- Connect KY-015 temperature and humidity sensor.
- Connect I2C LCD display.
- Connect rotation sensor as manual threshold input.
- Read temperature and humidity values.
- Show values locally on LCD.
- Send environment data to Raspberry Pi using MQTT.

Expected output:
- Temperature and humidity readings.
- LCD display output.
- MQTT messages to Raspberry Pi.

### 3. Moaj Chowdhury — ESP32-2 Fire and Gas Safety Node
Folder: `03_ESP32_FireGas_Safety_Node_MoajChowdhury/`

Main tasks:
- Connect MQ-2 smoke/gas sensor.
- Connect KY-026 analog flame sensor.
- Connect KY-006 passive buzzer.
- Connect KY-016 RGB LED.
- Detect gas/smoke and flame/fire.
- Turn on buzzer and warning LED during danger.
- Send alert data to Raspberry Pi using MQTT.

Expected output:
- Gas/smoke detection status.
- Flame/fire detection status.
- Local buzzer and RGB LED warning.
- MQTT alert messages.

### 4. Deepak Kapil — Arduino Uno WiFi Rev2 Actuator / Relay Node
Folder: `04_Arduino_Relay_Node_DeepakKapil/`

Main tasks:
- Connect KY-019 relay module.
- Use relay to simulate actuator control with an LED.
- Receive ON/OFF command from Raspberry Pi using Wi-Fi/MQTT.
- Switch relay ON/OFF.
- In future version, replace LED simulation with water pump, fan, or light.

Expected output:
- Relay ON/OFF control.
- Actuator simulation using LED.
- MQTT command reception.

## Repository Folder Structure

```text
Smart_Greenhouse_Team_Project/
├── README.md
├── docs/
│   └── system_overview.md
├── scripts/
│   └── update_dashboard.py
├── .github/
│   └── workflows/
│       └── update-dashboard.yml
├── 01_RaspberryPi_Central_Controller_MdMostafizurRahman/
│   ├── README.md
│   ├── src/
│   ├── logs/
│   └── docs/
├── 02_ESP32_Environment_Node_TurjaBarua/
│   ├── README.md
│   ├── src/
│   └── docs/
├── 03_ESP32_FireGas_Safety_Node_MoajChowdhury/
│   ├── README.md
│   ├── src/
│   └── docs/
└── 04_Arduino_Relay_Node_DeepakKapil/
    ├── README.md
    ├── src/
    └── docs/
```

## MQTT Topic Plan

```text
greenhouse/env/temperature
greenhouse/env/humidity
greenhouse/env/threshold
greenhouse/safety/gas
greenhouse/safety/flame
greenhouse/safety/alarm
greenhouse/actuator/relay/set
greenhouse/actuator/relay/status
```

## How the Dashboard Auto-Sync Works

1. A member commits changes inside their own folder.
2. GitHub Actions runs `.github/workflows/update-dashboard.yml`.
3. The workflow runs `scripts/update_dashboard.py`.
4. The script checks Git commit history for each folder.
5. The table between `AUTO_DASHBOARD_START` and `AUTO_DASHBOARD_END` is updated.
6. The updated README is committed automatically.

## Progress Rule

Progress is estimated by number of commits in each member folder:

```text
0 commits   = 0%
1 commit    = 20%
2 commits   = 40%
3 commits   = 60%
4 commits   = 80%
5+ commits  = 100%
```

This is a simple student-project progress indicator. Final progress should also be confirmed manually by testing the hardware.
