# ESP32-1 — Environment Monitoring Node

**Assigned to:** Turja Barua  
**Node:** ESP32-1 — Temperature, Humidity, LCD, Rotation Sensor

---

## Task Overview

You are responsible for the environment sensing node. This ESP32 reads temperature and humidity from the KY-015 sensor, displays the values locally on the I2C LCD, and publishes the data to the Raspberry Pi via MQTT over Wi-Fi. The rotation sensor is used as a manual input for setting an alarm threshold.

---

## Hardware

| Component | Module | Connection |
|-----------|--------|------------|
| KY-015 Temp & Humidity sensor | DHT11 combo sensor | Digital GPIO pin |
| I2C LCD display | 16x2 LCD with I2C backpack | SDA / SCL (GPIO 21 / 22) |
| Rotation sensor | Analog potentiometer module | ADC pin (GPIO 34) |
| KY-051 voltage level shifter | Signal translator | Between 5V LCD and 3.3V ESP32 |

---

## Software / Tools

- **Platform:** Arduino IDE or PlatformIO
- **Board:** ESP32 (select "ESP32 Dev Module")
- **Libraries:**
  - `DHT sensor library` (Adafruit)
  - `LiquidCrystal_I2C`
  - `PubSubClient` (MQTT)
  - `WiFi.h` (built-in)

---

## Responsibilities

1. Wire the KY-015, I2C LCD, and rotation sensor to the ESP32
2. Write Arduino sketch to read temperature and humidity from KY-015
3. Display live temperature and humidity values on the I2C LCD
4. Read the rotation sensor value and map it to an alarm threshold (e.g. 15–40 °C range)
5. Connect to Wi-Fi and publish all values to the Raspberry Pi MQTT broker
6. Test that data appears in the Raspberry Pi monitoring terminal

---

## MQTT Topics — Publish

```text
greenhouse/temperature   →  float value, e.g. "24.5"
greenhouse/humidity      →  float value, e.g. "62.0"
greenhouse/rotation      →  integer threshold value, e.g. "30"
```

---

## File Structure

```
esp32_env/
├── README.md              ← this file
├── wiring_diagram.md      ← pin connections and wiring notes
└── esp32_env_node/
    └── esp32_env_node.ino ← main Arduino sketch
```

---

## Deliverables

- [ ] KY-015 correctly wired and reading temperature and humidity
- [ ] I2C LCD displaying live sensor values
- [ ] Rotation sensor value mapped to a threshold and published via MQTT
- [ ] ESP32 connecting to Wi-Fi and broker successfully
- [ ] Data visible in the Raspberry Pi monitoring terminal
- [ ] Wiring diagram or photo of the assembled node

---

## Important Notes

- The KY-015 uses **3.3 V logic** — safe to connect directly to ESP32 GPIO.
- The I2C LCD typically runs at **5 V**. Use the **KY-051 level shifter** on the SDA/SCL lines.
- Get the **Raspberry Pi IP address** from Mostafizur to configure the MQTT broker address in your sketch.
- Publish data every **2–5 seconds** to avoid flooding the broker.
- Default MQTT broker port: **1883**.
