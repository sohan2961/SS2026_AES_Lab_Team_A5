# 01_RaspberryPi_Central_Controller_MdMostafizurRahman

# Raspberry Pi 3 — Central Controller

**Assigned to:** Md Mostafizur Rahman  
**Node:** Central Controller / MQTT Broker / Flask Dashboard  
**Status:** ✅ Fully implemented and tested

---

## What This Node Does

The Raspberry Pi 3 is the brain of the Smart Greenhouse system. It:

- Runs the **Mosquitto MQTT broker** that all other nodes connect to
- Receives live sensor data from ESP32-1, ESP32-2, and the Arduino node
- Displays all sensor values on a **live web dashboard**
- Logs every sensor reading to a **CSV file** with timestamps
- Sends **relay ON/OFF commands** to the Arduino actuator node

---

## How to View the Dashboard

### From any device on the same Wi-Fi network

1. Make sure the Raspberry Pi is powered on and connected to Wi-Fi
2. Open a browser on your phone, laptop, or PC
3. Go to:

```
http://172.20.10.2:5000
```

> If the IP address has changed, SSH into the Pi and run `hostname -I` to get the current IP.

You will see the live dashboard with:

| Section | What it shows |
|---------|--------------|
| 🌡 Temperature card | Live °C reading from ESP32-1 |
| 💧 Humidity card | Live % reading from ESP32-1 |
| 💨 Smoke / Gas card | Live ADC value from ESP32-2 |
| 🔥 Flame card | Flame detected / Clear from ESP32-2 |
| 🎛 Threshold card | Alarm threshold set by rotation sensor |
| Live chart | 60-point rolling graph — switch between Temp / Humidity / Smoke |
| Relay control | Turn the Arduino relay ON or OFF |
| Activity log | Last 20 MQTT messages in real time |
| ⚠ Alarm banner | Flashes red automatically when flame or alarm = 1 |

### Broker status indicator

Top right of the dashboard shows a green dot when the MQTT broker is connected and receiving data, or a red dot if the broker is offline.

---

## How to Test Without ESP32 or Arduino

SSH into the Pi and send fake sensor data from the terminal:

```bash
mosquitto_pub -h localhost -t greenhouse/temperature -m "24.5"
mosquitto_pub -h localhost -t greenhouse/humidity    -m "62.0"
mosquitto_pub -h localhost -t greenhouse/smoke       -m "350"
mosquitto_pub -h localhost -t greenhouse/flame       -m "1"
mosquitto_pub -h localhost -t greenhouse/rotation    -m "30"
mosquitto_pub -h localhost -t greenhouse/alarm       -m "1"
```

The dashboard updates within 3 seconds of each command.

---

## Hardware

| Component | Detail |
|-----------|--------|
| Raspberry Pi 3 Model B V1.2 | Central controller |
| Micro SD card | Raspberry Pi OS 64-bit + data storage |
| Wi-Fi (built-in) | MQTT broker + dashboard server |

---

## Software Stack

| Software | Version | Purpose |
|----------|---------|---------|
| Raspberry Pi OS | 64-bit (Debian Trixie) | Operating system |
| Mosquitto | Latest | MQTT broker on port 1883 |
| Python | 3.11 | Backend language |
| Flask | 3.1.3 | Web dashboard server |
| paho-mqtt | 2.1.0 | MQTT subscriber client |
| Chart.js | 4.4.0 | Live sensor charts |

---

## File Structure

```
src/
├── app.py          ← Flask app, MQTT subscriber, CSV logger
├── index.html      ← Dashboard HTML + JavaScript
├── style.css       ← Dashboard stylesheet
└── .gitignore      ← Excludes venv, logs, cache from Git
```

> Note: `templates/` and `static/` folders are used at runtime on the Pi.
> Files are stored flat in `src/` in this repository for simplicity.

---

## MQTT Topics

| Topic | Direction | Description |
|-------|-----------|-------------|
| `greenhouse/temperature` | Subscribe | Temperature from ESP32-1 |
| `greenhouse/humidity` | Subscribe | Humidity from ESP32-1 |
| `greenhouse/rotation` | Subscribe | Alarm threshold from ESP32-1 |
| `greenhouse/smoke` | Subscribe | Smoke/gas ADC value from ESP32-2 |
| `greenhouse/flame` | Subscribe | Flame detection flag from ESP32-2 |
| `greenhouse/alarm` | Subscribe | Global alarm state from ESP32-2 |
| `greenhouse/pump` | Publish | Relay ON/OFF command to Arduino |

---

## Setup Instructions (from scratch)

### 1. Flash the SD card
- Use **Raspberry Pi Imager**
- OS: Raspberry Pi OS 64-bit
- Configure hostname, SSH, Wi-Fi, username in the imager settings

### 2. SSH into the Pi
```bash
ssh sohan@sohan.local
```

### 3. Install dependencies
```bash
sudo apt update && sudo apt full-upgrade -y
sudo apt install -y mosquitto mosquitto-clients python3-pip python3-venv
```

### 4. Configure MQTT broker
```bash
sudo nano /etc/mosquitto/mosquitto.conf
```
Add at the bottom:
```
listener 1883
allow_anonymous true
```
```bash
sudo systemctl enable mosquitto
sudo systemctl restart mosquitto
```

### 5. Set up Python environment
```bash
mkdir -p ~/rpi_central/{templates,static,logs}
cd ~/rpi_central
python3 -m venv venv
source venv/bin/activate
pip install flask paho-mqtt
```

### 6. Copy files from this repo
```bash
cp src/app.py        ~/rpi_central/app.py
cp src/index.html    ~/rpi_central/templates/index.html
cp src/style.css     ~/rpi_central/static/style.css
```

### 7. Run the dashboard
```bash
cd ~/rpi_central
source venv/bin/activate
python3 app.py
```

Open browser at `http://<Pi-IP>:5000`

### 8. Enable auto-start on boot
```bash
sudo nano /etc/systemd/system/greenhouse.service
```
Paste:
```ini
[Unit]
Description=Greenhouse Flask Dashboard
After=network.target mosquitto.service

[Service]
User=sohan
WorkingDirectory=/home/sohan/rpi_central
ExecStart=/home/sohan/rpi_central/venv/bin/python3 /home/sohan/rpi_central/app.py
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
```
```bash
sudo systemctl daemon-reload
sudo systemctl enable greenhouse
sudo systemctl start greenhouse
```

---

## Useful Commands

| Task | Command |
|------|---------|
| Check dashboard service | `sudo systemctl status greenhouse` |
| View live logs | `sudo journalctl -u greenhouse -f` |
| Restart dashboard | `sudo systemctl restart greenhouse` |
| Get Pi IP address | `hostname -I` |
| Check broker status | `sudo systemctl status mosquitto` |
| Send test message | `mosquitto_pub -h localhost -t greenhouse/temperature -m "25.0"` |
| Download sensor log | Open `http://172.20.10.2:5000/api/log/download` in browser |

---

## Network Info for Team

| Detail | Value |
|--------|-------|
| Pi IP address | `172.20.10.2` |
| MQTT broker port | `1883` |
| Dashboard URL | `http://172.20.10.2:5000` |
| MQTT broker address (for ESP32/Arduino sketches) | `172.20.10.2` |

> All team members must connect their devices to the **same Wi-Fi network** as the Raspberry Pi.

---

## Deliverables Checklist

- [x] Raspberry Pi OS flashed and SSH working
- [x] Mosquitto MQTT broker installed and running
- [x] Flask dashboard running on port 5000
- [x] Live sensor cards updating in real time
- [x] Relay ON/OFF control working
- [x] CSV data logging to `logs/sensor_log.csv`
- [x] Auto-start on boot via systemd
- [x] Tested with simulated sensor data
- [x] Code pushed to GitHub
