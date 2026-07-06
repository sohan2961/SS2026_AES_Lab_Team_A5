# 02_ESP32_Environment_Node_TurjaBarua

**Owner:** Turja Barua 
**Modified by:** Md Mostafizur Rahman  
**Project:** Smart Greenhouse Monitoring System  
**Node type:** ESP32 Environment Monitoring Node  
**Updated function:** Temperature, humidity, regulator threshold, LCD display, and MQTT publishing for automatic relay control

---

## 1. Project Overview

This folder contains the ESP32 environment monitoring node for the Smart Greenhouse Monitoring System.

The ESP32 reads:

- Temperature from the KY-015/DHT11 sensor
- Humidity from the KY-015/DHT11 sensor
- Manual threshold temperature from the rotation sensor / potentiometer

The ESP32 shows the live values on the I2C LCD and publishes the same values to the Raspberry Pi MQTT broker. The Raspberry Pi dashboard then compares the real temperature with the threshold value from the regulator.

The relay is not controlled directly by this ESP32. Instead, this ESP32 sends the temperature and threshold values to the Raspberry Pi. The Raspberry Pi decides whether the relay should turn ON or OFF and sends the relay command to the Arduino relay node.

---

## 2. Main Application

The main application of this node is to monitor the greenhouse environment and allow the user to set a manual temperature threshold using the regulator.

The threshold works like this:

```text
Temperature > Regulator threshold  → Raspberry Pi sends relay ON
Temperature <= Regulator threshold → Raspberry Pi sends relay OFF
```

Example:

```text
Temperature = 31°C
Regulator threshold = 28°C
Result = Relay ON
```

```text
Temperature = 25°C
Regulator threshold = 28°C
Result = Relay OFF
```

This can simulate automatic fan, pump, or ventilation control.

---

## 3. Main Features

- Reads temperature using KY-015/DHT11.
- Reads humidity using KY-015/DHT11.
- Reads the rotation sensor as a manual temperature threshold.
- Converts the raw regulator value into a temperature threshold.
- Displays temperature, humidity, threshold, and status on a 16x2 I2C LCD.
- Publishes dashboard-compatible MQTT topics.
- Publishes structured project MQTT topics.
- Keeps the LCD running even if MQTT is temporarily offline.
- Works together with the Raspberry Pi dashboard and Arduino relay node.
- Supports automatic relay control through Raspberry Pi decision logic.

---

## 4. Hardware Used

| Component | Purpose |
|---|---|
| ESP32 development board | Main microcontroller and Wi-Fi node |
| KY-015 / DHT11 temperature and humidity sensor | Measures temperature and humidity |
| I2C LCD display, 16x2 | Shows local sensor and threshold values |
| Rotation sensor / potentiometer | Manual threshold temperature controller |
| Raspberry Pi 3 | MQTT broker and Flask dashboard |
| Arduino Uno WiFi Rev2 | Relay actuator node |
| KY-019 relay module | Actuator output controlled by Arduino |
| Jumper wires and breadboard | Circuit connection |

---

## 5. Pin Connection

| Component | Pin on component | ESP32 pin | Purpose |
|---|---:|---:|---|
| KY-015 / DHT11 | S / DATA / OUT | GPIO 4 | DHT11 data input |
| KY-015 / DHT11 | + / VCC | 3.3V | Sensor power |
| KY-015 / DHT11 | - / GND | GND | Ground |
| I2C LCD | SDA | GPIO 21 | I2C data |
| I2C LCD | SCL | GPIO 22 | I2C clock |
| I2C LCD | VCC | 3.3V or 5V | LCD power |
| I2C LCD | GND | GND | Ground |
| Rotation sensor | SIG / OUT | GPIO 34 | Analog threshold input |
| Rotation sensor | + / VCC | 3.3V | Sensor power |
| Rotation sensor | - / GND | GND | Ground |

> For ESP32, using 3.3V for sensor signal modules is the safer option.

---

## 6. Software Requirements

Install these in Arduino IDE:

- ESP32 board package by Espressif Systems
- PubSubClient
- DHT sensor library
- Adafruit Unified Sensor
- LiquidCrystal I2C

The sketch uses these libraries:

```cpp
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
```

---

## 7. Configuration

Before uploading the code, update the Wi-Fi and MQTT settings:

```cpp
const char* WIFI_SSID     = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* MQTT_SERVER   = "172.20.10.4";
const uint16_t MQTT_PORT  = 1883;
```

Important:

```text
Do not publish real Wi-Fi passwords to GitHub.
Use placeholders before committing code.
```

---

## 8. Important Code Settings

```cpp
#define DHT_PIN       4
#define DHT_TYPE      DHT11
#define ROTATION_PIN  34

#define I2C_SDA       21
#define I2C_SCL       22
#define LCD_ADDRESS   0x27
```

The LCD address is set to `0x27` because the I2C scanner detected the LCD at that address.

---

## 9. Regulator Threshold Mapping

The rotation sensor gives an analog value from the ESP32 ADC.

Typical range:

```text
0 to 4095
```

This raw value should be converted into a useful temperature threshold.

Example code:

```cpp
int rawValue = analogRead(ROTATION_PIN);
int thresholdTemp = map(rawValue, 0, 4095, 20, 40);
```

Meaning:

```text
Regulator minimum → 20°C threshold
Regulator maximum → 40°C threshold
```

So the user can rotate the regulator to select the temperature limit.

---

## 10. MQTT Topics Published by ESP32

### Dashboard-compatible topics

These topics are used by the existing Raspberry Pi Flask dashboard.

| Topic | Example value | Meaning |
|---|---:|---|
| `greenhouse/temperature` | `26.4` | Current temperature in Celsius |
| `greenhouse/humidity` | `58.0` | Current humidity in percent |
| `greenhouse/rotation` | `31` | Threshold temperature from regulator |

### Structured project topics

These topics are clearer for documentation and future development.

| Topic | Example value | Meaning |
|---|---:|---|
| `greenhouse/env/temperature` | `26.4` | Current temperature in Celsius |
| `greenhouse/env/humidity` | `58.0` | Current humidity in percent |
| `greenhouse/env/threshold` | `31` | Manual threshold from rotation sensor |
| `greenhouse/env/status` | `NORMAL`, `TEMP_HIGH`, or `DHT_READ_ERROR` | Environment node status |
| `greenhouse/env/ip` | `172.20.10.x` | ESP32 IP address |

---

## 11. MQTT Topics Used by the Whole Control System

| Device | Topic | Direction | Purpose |
|---|---|---|---|
| ESP32 Environment Node | `greenhouse/temperature` | Publish | Sends current temperature to Raspberry Pi |
| ESP32 Environment Node | `greenhouse/humidity` | Publish | Sends current humidity to Raspberry Pi |
| ESP32 Environment Node | `greenhouse/rotation` | Publish | Sends regulator threshold to Raspberry Pi |
| Raspberry Pi Central Controller | `greenhouse/actuator/relay/set` | Publish | Sends ON/OFF command to Arduino relay |
| Arduino Relay Node | `greenhouse/actuator/relay/status` | Publish | Sends relay status back to Raspberry Pi |

---

## 12. Complete Working Logic

```text
KY-015/DHT11 sensor
        ↓
ESP32 reads temperature and humidity
        ↓
Rotation sensor sets threshold temperature
        ↓
ESP32 publishes temperature, humidity, and threshold through MQTT
        ↓
Raspberry Pi receives values
        ↓
Raspberry Pi compares:
    temperature > threshold ?
        ↓
If YES → publish relay ON
If NO  → publish relay OFF
        ↓
Arduino receives relay command
        ↓
KY-019 relay switches actuator LED / fan / pump
```

---

## 13. Expected LCD Output

Normal condition:

```text
T:26.4C H:58%
Set:31C NORMAL
```

Temperature high condition:

```text
T:38.5C H:55%
Set:31C TEMP_HIGH
```

DHT sensor error:

```text
DHT read error
Check KY-015
```

MQTT offline but LCD still working:

```text
T:26.4C H:58%
Set:31C MQTT OFF
```

---

## 14. Expected Serial Monitor Output

Set Serial Monitor to **115200 baud**.

```text
Wi-Fi connected. ESP32 IP: 172.20.10.2
MQTT connected.
greenhouse/temperature = 26.4
greenhouse/humidity = 58.0
greenhouse/rotation = 31
LCD updated: T=26.4 C, H=58.0 %, Threshold=31 C, MQTT=ONLINE
```

If temperature is higher than the threshold:

```text
Status: TEMP_HIGH
```

If MQTT is disconnected:

```text
MQTT offline, LCD still active
```

---

## 15. Raspberry Pi MQTT Test

On the Raspberry Pi, subscribe to all greenhouse messages:

```bash
mosquitto_sub -h localhost -t 'greenhouse/#' -v
```

After resetting the ESP32, the terminal should show:

```text
greenhouse/temperature 26.4
greenhouse/humidity 58.0
greenhouse/rotation 31
greenhouse/env/status NORMAL
```

---

## 16. Automatic Relay Control Test

This test checks if the Raspberry Pi and Arduino relay node react correctly to the ESP32 values.

Open terminal 1 on Raspberry Pi:

```bash
mosquitto_sub -t greenhouse/actuator/relay/set -v
```

Open terminal 2 and simulate threshold and temperature:

```bash
mosquitto_pub -t greenhouse/rotation -m "28"
mosquitto_pub -t greenhouse/temperature -m "25"
```

Expected command:

```text
greenhouse/actuator/relay/set OFF
```

Now send a higher temperature:

```bash
mosquitto_pub -t greenhouse/temperature -m "31"
```

Expected command:

```text
greenhouse/actuator/relay/set ON
```

Now increase the threshold:

```bash
mosquitto_pub -t greenhouse/rotation -m "35"
```

Expected command:

```text
greenhouse/actuator/relay/set OFF
```

This proves that the regulator threshold controls the relay automatically.

---

## 17. Dashboard

Open the Raspberry Pi dashboard in a browser:

```text
http://172.20.10.4:5000
```

The dashboard should show:

- Temperature
- Humidity
- Rotation / threshold value
- Alarm or status
- Relay status
- Recent MQTT log

The relay button can still be used manually, but automatic mode controls the relay based on:

```text
Temperature > Regulator threshold
```

---

## 18. Troubleshooting

### LCD is blank

- Check SDA is connected to GPIO 21.
- Check SCL is connected to GPIO 22.
- Check VCC and GND.
- Adjust the small contrast screw on the LCD module.
- Run an I2C scanner and confirm the address is `0x27`.

### DHT read failed

- Check KY-015 DATA/S/OUT is connected to GPIO 4.
- Check KY-015 VCC is connected to 3.3V.
- Check KY-015 GND is connected to GND.
- Make sure the code uses `DHT11`.
- Try another jumper wire or another GPIO pin if necessary.

### Regulator value does not change

- Check rotation sensor SIG/OUT is connected to GPIO 34.
- Check VCC is connected to 3.3V.
- Check GND is connected to GND.
- Print the raw ADC value in Serial Monitor.
- Make sure the code uses `analogRead(ROTATION_PIN)`.

### MQTT is offline

- Check Raspberry Pi IP address.
- Check Mosquitto is running:

```bash
sudo systemctl status mosquitto
```

- Make sure ESP32 and Raspberry Pi are on the same Wi-Fi network.
- Make sure Mosquitto allows external connections on port 1883.
- Check MQTT from Raspberry Pi:

```bash
mosquitto_sub -h localhost -t 'greenhouse/#' -v
```

### Dashboard does not update

- Check the Raspberry Pi dashboard service:

```bash
sudo systemctl status greenhouse.service
```

- Restart the dashboard:

```bash
sudo systemctl restart greenhouse.service
```

- Check the dashboard file is active:

```bash
sudo systemctl status greenhouse.service
```

The active file should be:

```text
/home/sohan/rpi_central/app.py
```

---

## 19. GitHub Update Commands

After editing this node, update GitHub from the project root:

```bash
cd /home/sohan/SS2026_AES_Lab_Team_A5

git status
git add 02_ESP32_Environment_Node_TurjaBarua/
git commit -m "esp32_environment: update threshold MQTT documentation"
git pull --rebase origin main
git push origin main
```

---

## 20. Short Presentation Explanation

> This ESP32 node measures the greenhouse temperature and humidity using the KY-015 sensor. It also reads the rotation sensor, which works as a manual temperature threshold controller. The values are shown locally on the LCD and sent to the Raspberry Pi using MQTT. The Raspberry Pi compares the actual temperature with the threshold from the regulator. If the temperature is higher than the threshold, the Raspberry Pi sends an ON command to the Arduino relay node. If the temperature is lower or equal, it sends an OFF command. In this way, the greenhouse can automatically control an actuator like a fan, pump, or light.

---

## 21. Commit Message Example

```text
02_ESP32_Environment_Node_TurjaBarua: update ESP32 environment README with regulator threshold control
```
