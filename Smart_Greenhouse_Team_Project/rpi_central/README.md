# Raspberry Pi 3 — Central Controller

**Assigned to:** Md Mostafizur Rahman  
**Node:** Central Controller / MQTT Broker / Dashboard

---

## Task Overview

You are responsible for the brain of the system. The Raspberry Pi 3 runs the MQTT broker, receives all sensor data from the three peripheral nodes, logs data to the SD card, and acts as the system monitoring terminal.

---

## Hardware

| Component | Detail |
|-----------|--------|
| Raspberry Pi 3 Model B V1.2 | Central controller |
| Micro SD card | OS + data storage |
| Wi-Fi (built-in) | MQTT communication with all nodes |

---

## Software / Tools

- **OS:** Raspberry Pi OS Lite or Desktop
- **MQTT Broker:** Mosquitto (`sudo apt install mosquitto mosquitto-clients`)
- **Language:** Python 3
- **Libraries:** `paho-mqtt`, `csv` or `sqlite3` for logging

---

## Responsibilities

1. Install and configure the Mosquitto MQTT broker on the Raspberry Pi
2. Write a Python subscriber script that listens to all sensor topics
3. Print incoming data to a live monitoring terminal
4. Log all sensor readings to a CSV file on the SD card with timestamps
5. Publish relay control commands to `greenhouse/pump` when needed
6. Test end-to-end communication with all three nodes

---

## MQTT Topics — Subscribe

```text
greenhouse/temperature
greenhouse/humidity
greenhouse/smoke
greenhouse/flame
greenhouse/rotation
greenhouse/alarm
```

## MQTT Topics — Publish

```text
greenhouse/pump    →  payload: "ON" or "OFF"
```

---

## File Structure

```
rpi_central/
├── README.md           ← this file
├── broker_setup.md     ← Mosquitto install and config steps
├── monitor.py          ← main Python subscriber and logger
└── logs/
    └── sensor_log.csv  ← auto-generated data log (timestamped)
```

---

## Deliverables

- [ ] Mosquitto broker running and reachable on the local Wi-Fi network
- [ ] `monitor.py` receiving live data from all 3 nodes
- [ ] CSV log file with timestamped sensor entries
- [ ] Relay ON/OFF command successfully received by the Arduino node
- [ ] Screenshot or short video of the monitoring terminal in operation

---

## Important Notes

- Share the **Raspberry Pi IP address** with all team members so nodes can connect to the broker.
- Default Mosquitto port is **1883** (no TLS needed for local testing).
- All devices (Pi, ESP32s, Arduino) must be on the **same Wi-Fi network**.
- Broker must be running **before** any node tries to connect.
