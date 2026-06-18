# ESP32 Environment Monitoring Node — Turja Barua

## Project Overview

This folder contains the ESP32 environment monitoring node for the Smart Greenhouse Monitoring System. The node reads temperature and humidity from a KY-015/DHT11 sensor, reads a manual threshold value from a rotation sensor, shows the live values on an I2C LCD, and sends the same values to the Raspberry Pi dashboard using MQTT.

The code is designed so that the LCD continues to work even if the MQTT broker is temporarily offline. When MQTT is connected, the same sensor data is published to the Raspberry Pi dashboard.

## Main Features

- Reads temperature and humidity using the KY-015/DHT11 sensor.
- Displays temperature, humidity, threshold, and status on a 16x2 I2C LCD.
- Reads a rotation sensor as a manual temperature threshold input.
- Connects to Wi-Fi using ESP32 built-in Wi-Fi.
- Publishes sensor values to the Raspberry Pi MQTT broker.
- Supports the existing Flask dashboard topics.
- Uses non-blocking MQTT reconnect logic, so LCD display is not stopped by MQTT failure.

## Hardware Used

| Component | Purpose |
|---|---|
| ESP32 development board | Main microcontroller and Wi-Fi node |
| KY-015 temperature/humidity sensor | Measures temperature and humidity |
| I2C LCD display, 16x2 | Shows local values |
| Rotation sensor / potentiometer | Sets manual temperature threshold |
| Raspberry Pi | MQTT broker and online dashboard |
| Jumper wires and breadboard | Circuit connection |

## Pin Connection

| Component | Pin on component | ESP32 pin | Purpose |
|---|---:|---:|---|
| KY-015 | S / DATA / OUT | GPIO 4 | DHT11 data input |
| KY-015 | + / VCC | 3.3V | Sensor power |
| KY-015 | - / GND | GND | Ground |
| I2C LCD | SDA | GPIO 21 | I2C data |
| I2C LCD | SCL | GPIO 22 | I2C clock |
| I2C LCD | VCC | 3.3V or 5V | LCD power |
| I2C LCD | GND | GND | Ground |
| Rotation sensor | SIG / OUT | GPIO 34 | Analog threshold input |
| Rotation sensor | + / VCC | 3.3V | Sensor power |
| Rotation sensor | - / GND | GND | Ground |

> For ESP32, using 3.3V for sensor signal modules is the safer option.

## Software Requirements

Install the following in Arduino IDE:

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

## Configuration

Before uploading, update the Wi-Fi and MQTT settings in the sketch:

```cpp
const char* WIFI_SSID     = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* MQTT_SERVER   = "172.20.10.4";
const uint16_t MQTT_PORT  = 1883;
```

Do not publish real Wi-Fi passwords to GitHub. Replace private credentials with placeholders before committing.

## Important Code Settings

```cpp
#define DHT_PIN       4
#define DHT_TYPE      DHT11
#define ROTATION_PIN  34
#define I2C_SDA       21
#define I2C_SCL       22
#define LCD_ADDRESS   0x27
```

The LCD address is set to `0x27` because the I2C scanner detected the LCD at that address.

## MQTT Topics Published

### Dashboard-compatible topics

| Topic | Example value | Meaning |
|---|---:|---|
| `greenhouse/temperature` | `26.4` | Temperature in Celsius |
| `greenhouse/humidity` | `58.0` | Humidity in percent |
| `greenhouse/rotation` | `31` | Threshold value in Celsius |

### Structured project topics

| Topic | Example value | Meaning |
|---|---:|---|
| `greenhouse/env/temperature` | `26.4` | Temperature in Celsius |
| `greenhouse/env/humidity` | `58.0` | Humidity in percent |
| `greenhouse/env/threshold` | `31` | Manual threshold from rotation sensor |
| `greenhouse/env/status` | `NORMAL`, `TEMP_HIGH`, or `DHT_READ_ERROR` | Environment node status |
| `greenhouse/env/ip` | ESP32 IP address | ESP32 network address |

## Expected LCD Output

When the sensor works correctly, the LCD should show:

```text
T:26.4°C H:58%
Set:31°C NORMAL
```

If the temperature is higher than the selected threshold, the LCD may show:

```text
T:38.5°C H:55%
Set:31°C TEMP_HIGH
```

If the KY-015/DHT11 sensor cannot be read, the LCD shows:

```text
DHT read error
Check KY-015
```

## Expected Serial Monitor Output

Set Serial Monitor to **115200 baud**.

```text
Wi-Fi connected. ESP32 IP: 172.20.10.2
MQTT connected.
greenhouse/temperature = 26.4
greenhouse/humidity = 58.0
greenhouse/rotation = 31
LCD updated: T=26.4 C, H=58.0 %, Threshold=31 C, MQTT=ONLINE
```

## Raspberry Pi MQTT Test

On the Raspberry Pi, run:

```bash
mosquitto_sub -h localhost -t 'greenhouse/#' -v
```

After resetting the ESP32, the terminal should show MQTT messages from the ESP32 node.

## Dashboard

Open the Raspberry Pi dashboard in a browser:

```text
http://172.20.10.4:5000
```

The dashboard should show the same temperature, humidity, and threshold values that are displayed on the LCD.

## Troubleshooting

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

### MQTT is offline

- Check the Raspberry Pi IP address.
- Check Mosquitto is running.
- Make sure ESP32 and Raspberry Pi are on the same network.
- Make sure Mosquitto allows external connections on port 1883.

## Commit Message Example

```text
02_ESP32_Environment_Node_TurjaBarua: add fixed ESP32 LCD MQTT environment node
```
