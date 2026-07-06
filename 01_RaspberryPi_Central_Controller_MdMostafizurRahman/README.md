# 🍓 01_RaspberryPi_Central_Controller_MdMostafizurRahman

# 🍓 Raspberry Pi 3 — Central Controller

**👤 Owner:** Md Mostafizur Rahman  
**🧠 Node:** Central Controller / MQTT Broker / Flask Dashboard  
**✅ Status:** Fully implemented and tested  
**🌐 Current Raspberry Pi IP:** `172.20.10.4`  
**🔗 Dashboard URL:** `http://172.20.10.4:5000`

---

## 🧠 What This Node Does

The Raspberry Pi 3 is the **brain** of the Smart Greenhouse system. It works as the central communication, dashboard, logging, and control unit.

It:

- 📡 Runs the **Mosquitto MQTT broker** on port `1883`
- 🌡 Receives live temperature data from the ESP32 environment node
- 💧 Receives live humidity data from the ESP32 environment node
- 🎛 Receives the regulator/rotation threshold value from the ESP32 node
- 💨 Receives smoke/gas data from the safety node
- 🔥 Receives flame/alarm data from the safety node
- 🖥 Displays all live values on a **Flask web dashboard**
- 📄 Logs MQTT sensor readings and relay commands into a CSV file
- 🔌 Sends relay ON/OFF commands to the Arduino actuator node
- ⚙️ Automatically turns the relay ON/OFF by comparing temperature with the regulator threshold

---

## ⚙️ Main Working Logic

The ESP32 environment node sends two important values to the Raspberry Pi:

| Icon | Value | MQTT topic | Meaning |
|---|---|---|---|
| 🌡 | Current temperature | `greenhouse/temperature` | Live temperature from KY-015/DHT11 |
| 🎛 | Threshold temperature | `greenhouse/rotation` | User-selected threshold from rotation sensor/regulator |

The Raspberry Pi compares these two values:

```text
🌡 Temperature > 🎛 Threshold  → 🔌 Relay ON
🌡 Temperature <= 🎛 Threshold → 🔌 Relay OFF
```

Example:

```text
🌡 Temperature = 31°C
🎛 Threshold from regulator = 28°C
🔌 Result = Relay ON
```

```text
🌡 Temperature = 25°C
🎛 Threshold from regulator = 28°C
🔌 Result = Relay OFF
```

The relay command is sent to the Arduino relay node using:

```text
greenhouse/actuator/relay/set
```

---

## 🖥 How to View the Dashboard

From any device connected to the same Wi-Fi network as the Raspberry Pi, open:

```text
http://172.20.10.4:5000
```

If the IP address changes, SSH into the Raspberry Pi and run:

```bash
hostname -I
```

The dashboard shows:

| Icon | Dashboard section | What it shows |
|---|---|---|
| 🌡 | Temperature card | Live temperature from ESP32 environment node |
| 💧 | Humidity card | Live humidity from ESP32 environment node |
| 💨 | Smoke / Gas card | Smoke or gas value from safety node |
| 🔥 | Flame card | Flame detection status |
| 🎛 | Threshold card | Regulator threshold temperature |
| 📈 | Live chart | Rolling history of sensor values |
| 🔌 | Relay control | Manual relay ON/OFF buttons |
| 📋 | Activity log | Recent MQTT messages |
| ⚠️ | Alarm banner | Warning if flame/alarm is active |
| 🟢/🔴 | Broker status | MQTT connected/offline indicator |

---

## 🧩 Hardware

| Icon | Component | Purpose |
|---|---|---|
| 🍓 | Raspberry Pi 3 Model B V1.2 | Central controller |
| 💾 | Micro SD card | Raspberry Pi OS and storage |
| 📶 | Built-in Wi-Fi | Dashboard and MQTT network connection |

---

## 💻 Software Stack

| Icon | Software | Purpose |
|---|---|---|
| 🐧 | Raspberry Pi OS 64-bit | Operating system |
| 📡 | Mosquitto | MQTT broker |
| 🐍 | Python 3 | Backend programming language |
| 🌐 | Flask | Web dashboard server |
| 📬 | paho-mqtt | MQTT client library |
| 🧾 | HTML/CSS/JavaScript | Frontend dashboard |
| 📊 | Chart.js | Live dashboard charts |

---

## 📁 Project File Structure

Repository folder:

```text
01_RaspberryPi_Central_Controller_MdMostafizurRahman/
└── src/
    ├── app.py       # 🐍 Flask app, MQTT subscriber, relay logic, CSV logger
    ├── index.html   # 🖥 Dashboard page
    ├── style.css    # 🎨 Dashboard styling
    └── .gitignore   # 🚫 Files ignored by Git
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

The running systemd service uses this file:

```text
/home/sohan/rpi_central/app.py
```

---

## 📡 MQTT Topics

### 📥 Topics received by Raspberry Pi

| Icon | Topic | Direction | Description |
|---|---|---|---|
| 🌡 | `greenhouse/temperature` | Subscribe | Temperature from ESP32 environment node |
| 💧 | `greenhouse/humidity` | Subscribe | Humidity from ESP32 environment node |
| 🎛 | `greenhouse/rotation` | Subscribe | Threshold temperature from regulator/rotation sensor |
| 💨 | `greenhouse/smoke` | Subscribe | Smoke/gas value from safety node |
| 🔥 | `greenhouse/flame` | Subscribe | Flame detection flag from safety node |
| 🚨 | `greenhouse/alarm` | Subscribe | Alarm state from safety node |
| 🔌 | `greenhouse/actuator/relay/status` | Subscribe | Relay status from Arduino node |

### 📤 Topics published by Raspberry Pi

| Icon | Topic | Direction | Description |
|---|---|---|---|
| 🔌 | `greenhouse/actuator/relay/set` | Publish | Relay ON/OFF command to Arduino actuator node |

> ⚠️ Important: The old topic `greenhouse/pump` is no longer used.  
> ✅ The correct relay command topic is `greenhouse/actuator/relay/set`.

---

## 🔌 Automatic Relay Control

The Raspberry Pi automatically controls the Arduino relay using the temperature and regulator threshold.

The automatic control logic is inside `app.py`:

```python
def auto_control_relay():
    temperature = float(state["temperature"])
    threshold = float(state["rotation"])

    if temperature > threshold:
        command = "ON"
    else:
        command = "OFF"

    mqtt_client.publish("greenhouse/actuator/relay/set", command)
```

This function runs whenever a new temperature value or threshold value is received.

The relay can still be controlled manually from the dashboard button. Manual and automatic commands use the same MQTT topic:

```text
greenhouse/actuator/relay/set
```

---

## 🧪 How to Test Without ESP32 or Arduino

Use Mosquitto commands on the Raspberry Pi.

### 👀 Watch all MQTT messages

```bash
mosquitto_sub -h localhost -t 'greenhouse/#' -v
```

### 📤 Send fake sensor values

```bash
mosquitto_pub -h localhost -t greenhouse/temperature -m "25"
mosquitto_pub -h localhost -t greenhouse/humidity -m "60"
mosquitto_pub -h localhost -t greenhouse/rotation -m "28"
mosquitto_pub -h localhost -t greenhouse/smoke -m "350"
mosquitto_pub -h localhost -t greenhouse/flame -m "0"
mosquitto_pub -h localhost -t greenhouse/alarm -m "0"
```

### 🔌 Test automatic relay OFF

```bash
mosquitto_pub -h localhost -t greenhouse/rotation -m "28"
mosquitto_pub -h localhost -t greenhouse/temperature -m "25"
```

Expected output on relay topic:

```text
greenhouse/actuator/relay/set OFF
```

### 🔌 Test automatic relay ON

```bash
mosquitto_pub -h localhost -t greenhouse/temperature -m "31"
```

Expected output:

```text
greenhouse/actuator/relay/set ON
```

### 🎛 Test regulator threshold change

```bash
mosquitto_pub -h localhost -t greenhouse/rotation -m "35"
```

Expected output:

```text
greenhouse/actuator/relay/set OFF
```

---

## 🛠 Setup Instructions From Scratch

### 1️⃣ Flash Raspberry Pi OS

Use Raspberry Pi Imager and select Raspberry Pi OS 64-bit.

Enable:

- 🔐 SSH
- 📶 Wi-Fi
- 👤 Username/password
- 🏷 Hostname

### 2️⃣ SSH into the Raspberry Pi

```bash
ssh sohan@172.20.10.4
```

Or, if hostname resolution works:

```bash
ssh sohan@sohan.local
```

### 3️⃣ Install dependencies

```bash
sudo apt update
sudo apt full-upgrade -y
sudo apt install -y mosquitto mosquitto-clients python3-pip python3-venv
```

### 4️⃣ Configure Mosquitto MQTT broker

Open the Mosquitto config file:

```bash
sudo nano /etc/mosquitto/mosquitto.conf
```

Add these lines at the bottom:

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

### 5️⃣ Prepare runtime folder

```bash
mkdir -p ~/rpi_central/{templates,static,logs}
cd ~/rpi_central
python3 -m venv venv
source venv/bin/activate
pip install flask paho-mqtt
```

### 6️⃣ Copy repository files to runtime folder

From the repository:

```bash
cp 01_RaspberryPi_Central_Controller_MdMostafizurRahman/src/app.py ~/rpi_central/app.py
cp 01_RaspberryPi_Central_Controller_MdMostafizurRahman/src/index.html ~/rpi_central/templates/index.html
cp 01_RaspberryPi_Central_Controller_MdMostafizurRahman/src/style.css ~/rpi_central/static/style.css
```

### 7️⃣ Run manually for testing

```bash
cd ~/rpi_central
source venv/bin/activate
python3 app.py
```

Open:

```text
http://172.20.10.4:5000
```

Stop manual running with:

```text
CTRL + C
```

### 8️⃣ Enable dashboard auto-start

Create systemd service:

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

---

## 🧰 Useful Commands

| Icon | Task | Command |
|---|---|---|
| ✅ | Check dashboard status | `sudo systemctl status greenhouse.service` |
| 🔄 | Restart dashboard | `sudo systemctl restart greenhouse.service` |
| 🛑 | Stop dashboard | `sudo systemctl stop greenhouse.service` |
| 📜 | View live dashboard logs | `sudo journalctl -u greenhouse.service -f` |
| ❌ | View recent errors | `sudo journalctl -u greenhouse.service -n 50 --no-pager` |
| 📡 | Check MQTT broker | `sudo systemctl status mosquitto` |
| 🌐 | Get Pi IP address | `hostname -I` |
| 👀 | Watch MQTT topics | `mosquitto_sub -h localhost -t 'greenhouse/#' -v` |
| 📥 | Download log | `http://172.20.10.4:5000/api/log/download` |

---

## 🌐 Network Information for Team

| Icon | Detail | Value |
|---|---|---|
| 🍓 | Raspberry Pi IP address | `172.20.10.4` |
| 📡 | MQTT broker port | `1883` |
| 🖥 | Dashboard URL | `http://172.20.10.4:5000` |
| 📶 | MQTT broker address for ESP32/Arduino | `172.20.10.4` |
| 🔌 | Relay command topic | `greenhouse/actuator/relay/set` |

All ESP32 and Arduino nodes must be connected to the same Wi-Fi network as the Raspberry Pi.

---

## 🧯 Troubleshooting

### 🖥 Dashboard is not opening

Check service:

```bash
sudo systemctl status greenhouse.service
```

Check logs:

```bash
sudo journalctl -u greenhouse.service -n 50 --no-pager
```

Restart service:

```bash
sudo systemctl restart greenhouse.service
```

### 📡 MQTT messages are not visible

Check Mosquitto:

```bash
sudo systemctl status mosquitto
```

Listen to all topics:

```bash
mosquitto_sub -h localhost -t 'greenhouse/#' -v
```

### 🔌 Relay button works but relay does not switch

Check that the dashboard publishes to:

```text
greenhouse/actuator/relay/set
```

Check Arduino wiring:

```text
KY-019 S  → Arduino D7
KY-019 +  → Arduino 5V
KY-019 -  → Arduino GND
```

Most KY-019 relay modules are active LOW. Arduino code may need:

```cpp
digitalWrite(RELAY_PIN, LOW);   // relay ON
digitalWrite(RELAY_PIN, HIGH);  // relay OFF
```

### ⚙️ Automatic relay does not work

Check that both values are being received:

```bash
mosquitto_sub -h localhost -t 'greenhouse/temperature' -v
mosquitto_sub -h localhost -t 'greenhouse/rotation' -v
```

Then test:

```bash
mosquitto_pub -h localhost -t greenhouse/rotation -m "28"
mosquitto_pub -h localhost -t greenhouse/temperature -m "31"
```

Expected:

```text
greenhouse/actuator/relay/set ON
```

---

## 🚀 GitHub Update Commands

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

## ✅ Deliverables Checklist

- [x] 🍓 Raspberry Pi OS flashed and SSH working
- [x] 📡 Mosquitto MQTT broker installed and running
- [x] 🖥 Flask dashboard running on port 5000
- [x] 🌐 Live dashboard available at `http://172.20.10.4:5000`
- [x] 🌡 Sensor cards update in real time
- [x] 📄 CSV data logging to `logs/sensor_log.csv`
- [x] 🔌 Manual relay control from dashboard works
- [x] ✅ Relay topic corrected to `greenhouse/actuator/relay/set`
- [x] ⚙️ Automatic relay control using temperature and regulator threshold works
- [x] 🔁 Auto-start on boot via systemd
- [x] 🧪 Tested with simulated MQTT sensor data
- [x] 🚀 Code pushed to GitHub

---

## 🎤 Final Project Explanation

The Raspberry Pi 3 works as the **central controller** of the Smart Greenhouse Monitoring System. It runs the MQTT broker and receives sensor data from the ESP32 nodes. The Flask dashboard shows the live values and stores them in a CSV log file. The regulator value from the ESP32 environment node is used as the temperature threshold. When the current temperature is higher than this threshold, the Raspberry Pi automatically sends an ON command to the Arduino relay node. When the temperature is lower than or equal to the threshold, it sends an OFF command. This creates an automatic actuator control system for the greenhouse.
