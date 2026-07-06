# 01_RaspberryPi_Central_Controller_MdMostafizurRahman

# Raspberry Pi 3 — Central Controller

**Owner:** Md Mostafizur Rahman  
**Node:** Central Controller / MQTT Broker / Flask Dashboard  
**Status:** ✅ Fully implemented, tested, and updated with automatic relay control

---

## What This Node Does

The Raspberry Pi 3 is the **brain** of the Smart Greenhouse system.

It performs five main jobs:

1. Runs the **Mosquitto MQTT broker** on port `1883`
2. Receives live sensor data from the ESP32 and Arduino nodes
3. Displays the values on a **Flask web dashboard**
4. Logs received data and relay commands into a CSV file
5. Sends relay ON/OFF commands to the Arduino actuator node

The latest version also supports **automatic relay control**:

```text
Temperature > regulator threshold  → Relay ON
Temperature <= regulator threshold → Relay OFF
```

The threshold is **not fixed in the code**.  
It comes from the ESP32 environment node through the rotation sensor / regulator.

---

## Updated Control Logic

The ESP32 environment node sends:

```text
greenhouse/temperature  → current temperature from KY-015/DHT11
greenhouse/humidity     → current humidity
greenhouse/rotation     → threshold temperature from regulator
```

The Raspberry Pi compares the live temperature with the regulator threshold.

Example:

```text
Temperature = 31°C
Threshold from regulator = 28°C
Result: Raspberry Pi publishes ON to Arduino relay node
```

Another example:

```text
Temperature = 25°C
Threshold from regulator = 28°C
Result: Raspberry Pi publishes OFF to Arduino relay node
```

The command is sent to:

```text
greenhouse/actuator/relay/set
```

The Arduino relay node receives this MQTT command and switches the KY-019 relay.

---

## How to View the Dashboard

### From any device on the same Wi-Fi network

1. Make sure the Raspberry Pi is powered on and connected to Wi-Fi.
2. Make sure the dashboard service is running.
3. Open a browser on your phone, laptop, or PC.
4. Go to:

```text
http://172.20.10.4:5000
```

> If the IP address changes, SSH into the Pi and run:

```bash
hostname -I
```

The dashboard shows:

| Section | What it shows |
|---|---|
| Temperature card | Live temperature in °C from ESP32 environment node |
| Humidity card | Live humidity in % from ESP32 environment node |
| Smoke / Gas card | Live ADC value from ESP32 safety node |
| Flame card | Flame detected / clear state from ESP32 safety node |
| Threshold card | Temperature threshold selected by regulator |
| Live chart | Rolling graph of received sensor values |
| Relay control | Manual ON/OFF control for Arduino relay |
| Activity log | Recent MQTT messages |
| Alarm banner | Warning state when flame/alarm is detected |
| Broker indicator | Shows MQTT broker connection status |

---

## Hardware

| Component | Detail |
|---|---|
| Raspberry Pi 3 Model B V1.2 | Central controller |
| Micro SD card | Raspberry Pi OS 64-bit and project storage |
| Built-in Wi-Fi | Network connection for MQTT and dashboard |
| Laptop / phone browser | Used to view dashboard |

---

## Software Stack

| Software | Purpose |
|---|---|
| Raspberry Pi OS 64-bit | Operating system |
| Mosquitto | MQTT broker |
| Python 3 | Backend programming language |
| Flask | Web dashboard server |
| paho-mqtt | MQTT client library |
| HTML/CSS/JavaScript | Dashboard frontend |
| Chart.js | Live sensor chart |
| systemd | Auto-start dashboard service |

---

## File Structure

Repository folder:

```text
01_RaspberryPi_Central_Controller_MdMostafizurRahman/
└── src/
    ├── app.py
    ├── index.html
    ├── style.css
    └── .gitignore
```

Runtime folder on the Raspberry Pi:

```text
/home/sohan/rpi_central/
├── app.py
├── templates/
│   └── index.html
├── static/
│   └── style.css
├── logs/
│   └── sensor_log.csv
└── venv/
```

Important:

```text
Repository app.py:
01_RaspberryPi_Central_Controller_MdMostafizurRahman/src/app.py

Active running app.py:
 /home/sohan/rpi_central/app.py
```

The service uses the active runtime file:

```text
/home/sohan/rpi_central/app.py
```

---

## MQTT Topics

### Topics received by Raspberry Pi

| Topic | Direction | Description |
|---|---|---|
| `greenhouse/temperature` | Subscribe | Temperature from ESP32 environment node |
| `greenhouse/humidity` | Subscribe | Humidity from ESP32 environment node |
| `greenhouse/rotation` | Subscribe | Regulator threshold value from ESP32 environment node |
| `greenhouse/smoke` | Subscribe | Smoke/gas value from ESP32 safety node |
| `greenhouse/flame` | Subscribe | Flame detection value from ESP32 safety node |
| `greenhouse/alarm` | Subscribe | Alarm status from ESP32 safety node |
| `greenhouse/actuator/relay/status` | Subscribe | Relay status from Arduino node |

### Topic published by Raspberry Pi

| Topic | Direction | Description |
|---|---|---|
| `greenhouse/actuator/relay/set` | Publish | Relay ON/OFF command to Arduino actuator node |

Old topic removed:

```text
greenhouse/pump
```

The dashboard must not publish to `greenhouse/pump` anymore.  
It now publishes relay commands to:

```text
greenhouse/actuator/relay/set
```

---

## Manual Relay Control

The dashboard ON/OFF button sends:

```text
ON  → greenhouse/actuator/relay/set
OFF → greenhouse/actuator/relay/set
```

Manual terminal test from Raspberry Pi:

```bash
mosquitto_pub -t greenhouse/actuator/relay/set -m "ON"
mosquitto_pub -t greenhouse/actuator/relay/set -m "OFF"
```

To watch the command:

```bash
mosquitto_sub -t greenhouse/actuator/relay/set -v
```

Expected output:

```text
greenhouse/actuator/relay/set ON
greenhouse/actuator/relay/set OFF
```

---

## Automatic Relay Control Test

Open Terminal 1 on the Raspberry Pi:

```bash
mosquitto_sub -t greenhouse/actuator/relay/set -v
```

Open Terminal 2 and publish a threshold and temperature:

```bash
mosquitto_pub -t greenhouse/rotation -m "28"
mosquitto_pub -t greenhouse/temperature -m "25"
```

Expected result:

```text
greenhouse/actuator/relay/set OFF
```

Now publish a higher temperature:

```bash
mosquitto_pub -t greenhouse/temperature -m "31"
```

Expected result:

```text
greenhouse/actuator/relay/set ON
```

Now increase the regulator threshold:

```bash
mosquitto_pub -t greenhouse/rotation -m "35"
```

Expected result:

```text
greenhouse/actuator/relay/set OFF
```

This proves that the relay is controlled automatically using the regulator threshold.

---

## Simulated Sensor Data Test

Use these commands to test the dashboard without real ESP32 sensors:

```bash
mosquitto_pub -h localhost -t greenhouse/temperature -m "24.5"
mosquitto_pub -h localhost -t greenhouse/humidity    -m "62.0"
mosquitto_pub -h localhost -t greenhouse/smoke       -m "350"
mosquitto_pub -h localhost -t greenhouse/flame       -m "0"
mosquitto_pub -h localhost -t greenhouse/rotation    -m "30"
mosquitto_pub -h localhost -t greenhouse/alarm       -m "0"
```

The dashboard should update within a few seconds.

---

## Setup Instructions From Scratch

### 1. Flash Raspberry Pi OS

Use **Raspberry Pi Imager**.

Recommended settings:

```text
OS: Raspberry Pi OS 64-bit
Hostname: sohan
Username: sohan
SSH: enabled
Wi-Fi: configured
```

---

### 2. SSH into the Pi

```bash
ssh sohan@sohan.local
```

If `.local` does not work, use the IP address:

```bash
ssh sohan@172.20.10.4
```

---

### 3. Install dependencies

```bash
sudo apt update
sudo apt full-upgrade -y
sudo apt install -y mosquitto mosquitto-clients python3-pip python3-venv
```

---

### 4. Configure Mosquitto

Open the configuration file:

```bash
sudo nano /etc/mosquitto/mosquitto.conf
```

Add this at the bottom:

```text
listener 1883
allow_anonymous true
```

Restart Mosquitto:

```bash
sudo systemctl enable mosquitto
sudo systemctl restart mosquitto
sudo systemctl status mosquitto
```

---

### 5. Set up Python environment

```bash
mkdir -p ~/rpi_central/{templates,static,logs}
cd ~/rpi_central
python3 -m venv venv
source venv/bin/activate
pip install flask paho-mqtt
```

---

### 6. Copy repository files to runtime folder

From the GitHub repository folder:

```bash
cp 01_RaspberryPi_Central_Controller_MdMostafizurRahman/src/app.py     ~/rpi_central/app.py
cp 01_RaspberryPi_Central_Controller_MdMostafizurRahman/src/index.html ~/rpi_central/templates/index.html
cp 01_RaspberryPi_Central_Controller_MdMostafizurRahman/src/style.css  ~/rpi_central/static/style.css
```

---

### 7. Run dashboard manually

```bash
cd ~/rpi_central
source venv/bin/activate
python3 app.py
```

Open:

```text
http://172.20.10.4:5000
```

Stop manual run with:

```text
CTRL + C
```

---

### 8. Create systemd service

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

Enable and start:

```bash
sudo systemctl daemon-reload
sudo systemctl enable greenhouse.service
sudo systemctl start greenhouse.service
```

Check:

```bash
sudo systemctl status greenhouse.service
```

---

## Useful Commands

| Task | Command |
|---|---|
| Check dashboard service | `sudo systemctl status greenhouse.service` |
| Restart dashboard | `sudo systemctl restart greenhouse.service` |
| Stop dashboard | `sudo systemctl stop greenhouse.service` |
| View live dashboard logs | `sudo journalctl -u greenhouse.service -f` |
| Show last 50 service logs | `sudo journalctl -u greenhouse.service -n 50 --no-pager` |
| Check broker status | `sudo systemctl status mosquitto` |
| Get Pi IP address | `hostname -I` |
| Subscribe to all greenhouse topics | `mosquitto_sub -t 'greenhouse/#' -v` |
| Download CSV log | `http://172.20.10.4:5000/api/log/download` |

---

## Troubleshooting

### Dashboard does not open

Check service:

```bash
sudo systemctl status greenhouse.service
```

Check logs:

```bash
sudo journalctl -u greenhouse.service -n 50 --no-pager
```

Restart:

```bash
sudo systemctl restart greenhouse.service
```

---

### Service fails after editing app.py

Run manually to see the exact Python error:

```bash
sudo systemctl stop greenhouse.service
cd /home/sohan/rpi_central
source venv/bin/activate
python3 app.py
```

Common issue:

```text
IndentationError
```

Fix indentation, then restart:

```bash
sudo systemctl restart greenhouse.service
```

---

### Relay does not respond

Watch the relay command topic:

```bash
mosquitto_sub -t greenhouse/actuator/relay/set -v
```

Then publish:

```bash
mosquitto_pub -t greenhouse/actuator/relay/set -m "ON"
mosquitto_pub -t greenhouse/actuator/relay/set -m "OFF"
```

If messages appear but the relay does not switch, check Arduino wiring and active LOW relay logic.

---

### ESP32 values do not appear

Subscribe to all MQTT topics:

```bash
mosquitto_sub -t 'greenhouse/#' -v
```

If no messages appear:

- Check ESP32 Wi-Fi
- Check Pi IP in ESP32 code
- Check Mosquitto service
- Make sure ESP32 and Pi are on the same Wi-Fi network

---

## Network Info for Team

| Detail | Current value |
|---|---|
| Pi IP address | `172.20.10.4` |
| MQTT broker port | `1883` |
| Dashboard URL | `http://172.20.10.4:5000` |
| MQTT broker address for ESP32/Arduino | `172.20.10.4` |
| Active runtime folder | `/home/sohan/rpi_central` |
| GitHub repository folder | `/home/sohan/SS2026_AES_Lab_Team_A5` |

> If the Pi IP changes, update the MQTT broker address in the ESP32 and Arduino sketches.

---

## Updating GitHub

After editing the active runtime file, copy it back into the repository:

```bash
cp /home/sohan/rpi_central/app.py /home/sohan/SS2026_AES_Lab_Team_A5/01_RaspberryPi_Central_Controller_MdMostafizurRahman/src/app.py
```

Then push:

```bash
cd /home/sohan/SS2026_AES_Lab_Team_A5
git status
git add 01_RaspberryPi_Central_Controller_MdMostafizurRahman/src/app.py
git commit -m "rpi_central: add automatic relay control using regulator threshold"
git pull --rebase origin main
git push origin main
```

---

## Deliverables Checklist

- [x] Raspberry Pi OS flashed and SSH working
- [x] Mosquitto MQTT broker installed and running
- [x] Flask dashboard running on port 5000
- [x] Live sensor cards updating in real time
- [x] CSV data logging to `logs/sensor_log.csv`
- [x] Manual relay ON/OFF control working
- [x] Correct relay command topic added: `greenhouse/actuator/relay/set`
- [x] Automatic relay control using temperature and regulator threshold
- [x] Auto-start on boot via systemd
- [x] Tested with simulated MQTT sensor data
- [x] Code pushed to GitHub

---

## Short Explanation for Presentation

> The Raspberry Pi acts as the central controller of the Smart Greenhouse system. It runs the MQTT broker and Flask dashboard. The ESP32 environment node sends temperature, humidity, and regulator threshold values to the Pi. The Pi compares the temperature with the threshold. If the temperature is higher than the threshold, the Pi automatically sends an ON command to the Arduino relay node. If the temperature is lower or equal, it sends an OFF command. In this way, the Raspberry Pi controls the actuator automatically using MQTT communication.
