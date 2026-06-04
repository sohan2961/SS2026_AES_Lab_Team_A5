# 🌿 Smart Greenhouse Monitoring System with Fire and Gas Detection

> IoT embedded systems project · Raspberry Pi 3 + ESP32 + Arduino Uno WiFi Rev2 · MQTT over Wi-Fi

---

## 📊 Project Dashboard

Open `main/dashboard.html` in a browser and enter your GitHub repository name to see:

- Live commit activity for each team member
- Per-member progress bars and file counts
- Recent commit feed with folder tags
- 12-week contribution heatmap

The dashboard syncs automatically with the GitHub API every 60 seconds. No token or login is required for public repositories.

---

## 👥 Team & Node Assignments

| Folder | Node | Assigned To |
|--------|------|-------------|
| [`rpi_central/`](./rpi_central/README.md) | Raspberry Pi 3 — Central Controller & MQTT Broker | **Md Mostafizur Rahman** |
| [`esp32_env/`](./esp32_env/README.md) | ESP32-1 — Environment Monitoring Node | **Turja Barua** |
| [`esp32_fire/`](./esp32_fire/README.md) | ESP32-2 — Fire & Gas Safety Node | **Moaj Chowdhury** |
| [`arduino_actuator/`](./arduino_actuator/README.md) | Arduino Uno WiFi Rev2 — Actuator / Relay Node | **Deepak Kapil** |
| [`main/`](./main/) | Project dashboard, shared docs, system README | All members |

Each folder contains its own `README.md` with full task description, wiring guide, software requirements, MQTT topics, and a personal deliverables checklist.

---

## 🗂️ Repository Structure

```
smart-greenhouse/
│
├── main/
│   ├── dashboard.html          ← live project dashboard (open in browser)
│   └── README.md               ← this file
│
├── rpi_central/                ← Mostafizur
│   ├── README.md
│   ├── broker_setup.md
│   ├── monitor.py
│   └── logs/
│
├── esp32_env/                  ← Turja
│   ├── README.md
│   ├── wiring_diagram.md
│   └── esp32_env_node/
│       └── esp32_env_node.ino
│
├── esp32_fire/                 ← Moaj
│   ├── README.md
│   ├── wiring_diagram.md
│   └── esp32_fire_node/
│       └── esp32_fire_node.ino
│
└── arduino_actuator/           ← Deepak
    ├── README.md
    ├── wiring_diagram.md
    └── arduino_actuator_node/
        └── arduino_actuator_node.ino
```

---

## 🔧 Hardware Overview

| Component | Qty | Assigned Node |
|-----------|-----|---------------|
| Raspberry Pi 3 Model B V1.2 | 1 | Central controller |
| ESP32 microcontroller | 2 | Env. monitoring + Fire & Gas nodes |
| Arduino Uno WiFi Rev2 | 2 | Actuator node (1 active) |
| KY-015 Temp & Humidity sensor | 1 | ESP32-1 |
| MQ-2 Smoke/Gas sensor | 1 | ESP32-2 |
| KY-026 Flame sensor | 1 | ESP32-2 |
| KY-006 Passive buzzer | 1 | ESP32-2 |
| KY-016 RGB LED | 1 | ESP32-2 |
| KY-019 5V Relay module | 1 | Arduino |
| KY-051 Voltage level shifter | 1 | ESP32-1 (LCD signal) |
| I2C LCD display | 1 | ESP32-1 |
| Rotation sensor | 1 | ESP32-1 |
| Micro SD card | 1 | Raspberry Pi |
| Jumper wires & breadboard | — | All nodes |

---

## 📡 MQTT Topics

| Topic | Direction | Published By | Subscribed By |
|-------|-----------|--------------|---------------|
| `greenhouse/temperature` | sensor → broker | ESP32-1 | Raspberry Pi |
| `greenhouse/humidity` | sensor → broker | ESP32-1 | Raspberry Pi |
| `greenhouse/rotation` | sensor → broker | ESP32-1 | Raspberry Pi |
| `greenhouse/smoke` | sensor → broker | ESP32-2 | Raspberry Pi |
| `greenhouse/flame` | sensor → broker | ESP32-2 | Raspberry Pi |
| `greenhouse/alarm` | sensor → broker | ESP32-2 | Raspberry Pi |
| `greenhouse/pump` | command → actuator | Raspberry Pi | Arduino |

---

## 🚀 Getting Started

### Step 1 — Network setup (Mostafizur)
1. Connect the Raspberry Pi to the team Wi-Fi network.
2. Install and start the Mosquitto broker: `sudo apt install mosquitto && sudo systemctl enable mosquitto`.
3. Note the Pi's IP address and share it with the team: `hostname -I`.

### Step 2 — Each node member
1. Open your folder's `README.md` for full wiring and code instructions.
2. In your Arduino sketch, set the broker IP to the Raspberry Pi's address.
3. Use the same Wi-Fi SSID and password as the Raspberry Pi.
4. Commit your code with your name or folder in the commit message so the dashboard can track it.

### Step 3 — Dashboard
1. Create a GitHub repository and push all folders.
2. Open `main/dashboard.html` in any browser.
3. Enter `your-github-username/smart-greenhouse` in the repo field and click **Connect**.
4. The dashboard will start tracking commits and showing progress automatically.

---

## ✅ Overall Project Checklist

- [ ] **Mostafizur** — MQTT broker running, monitor.py live, CSV logging working
- [ ] **Turja** — KY-015 + LCD + rotation sensor publishing to broker
- [ ] **Moaj** — MQ-2 + KY-026 + buzzer + RGB LED alarm system working
- [ ] **Deepak** — Arduino relay node receiving commands and switching relay
- [ ] **All** — End-to-end test: all 3 nodes visible in Raspberry Pi terminal simultaneously
- [ ] **All** — Code pushed to GitHub with clean commit messages

---

## 📝 Commit Message Convention

Use your folder name in commit messages so the dashboard can automatically tag and track your work:

```
rpi_central: add CSV logging to monitor.py
esp32_env: fix KY-015 DHT read timeout
esp32_fire: tune MQ-2 smoke threshold to 400
arduino_actuator: add MQTT reconnect logic
main: update README hardware table
```

---

## 🔮 Future Improvements

- Connect a real water pump or fan to the relay output
- Add soil moisture sensing and automatic irrigation scheduling
- Web dashboard with live charts (Flask + Chart.js on Raspberry Pi)
- Mobile push notifications on alarm events
- Database logging with SQLite or InfluxDB
- Solar power and battery backup

---

*Smart Greenhouse Monitoring System — Embedded Systems / IoT Lab Project*
