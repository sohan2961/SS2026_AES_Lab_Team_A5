# 04_Arduino_Relay_Node_DeepakKapil

**Owner:** Deepak Kapil  
**Modified by:** Md Mostafizur Rahman  
**Project:** Smart Greenhouse IoT System  
**Device:** Arduino Uno WiFi Rev2  
**Actuator:** KY-019 5V Relay Module

---

## 1. Purpose

This node is the **actuator control node** of the Smart Greenhouse project.  
It receives MQTT commands from the Raspberry Pi central controller and switches the KY-019 relay **ON** or **OFF**.

In the current prototype, the relay is used to control an LED as a safe actuator simulation. In a future version, the same relay output can control a real greenhouse actuator such as:

- Water pump
- Cooling fan
- Grow light
- Alarm device

---

## 2. Role in the System

The Arduino relay node does not make the temperature decision itself.  
The Raspberry Pi central controller decides when the relay should turn ON or OFF.

The automatic control rule is:

```text
Temperature > regulator threshold  → Relay ON
Temperature <= regulator threshold → Relay OFF
```

The regulator threshold comes from the rotation sensor/potentiometer connected to the environment node. The Raspberry Pi compares this threshold with the current temperature and sends the relay command to this Arduino node.

---

## 3. Hardware Used

| Component | Purpose |
|---|---|
| Arduino Uno WiFi Rev2 | Receives MQTT commands over WiFi |
| KY-019 5V Relay Module | Switches actuator ON/OFF |
| LED + 220Ω resistor | Simulates the actuator output |
| Jumper wires | Connections |
| Raspberry Pi 3 | MQTT broker and dashboard |

---

## 4. Wiring

### KY-019 Relay to Arduino Uno WiFi Rev2

| KY-019 Relay Pin | Arduino Uno WiFi Rev2 Pin |
|---|---|
| S / IN | D7 |
| + / VCC | 5V |
| - / GND | GND |

### LED Actuator Simulation

```text
Arduino 5V  → Relay COM
Relay NO    → LED long leg/anode
LED short leg/cathode → 220Ω resistor → GND
```

Use the **NO** contact so the LED turns ON only when the relay is activated.

---

## 5. Important Relay Logic

The KY-019 relay module used in this project behaves as an **active LOW relay**.

That means:

```text
RELAY_PIN LOW  → Relay ON
RELAY_PIN HIGH → Relay OFF
```

Therefore, the Arduino code uses inverted relay control:

```cpp
digitalWrite(RELAY_PIN, LOW);   // Relay ON
digitalWrite(RELAY_PIN, HIGH);  // Relay OFF
```

The Arduino onboard LED on pin 13 is used as a status indicator:

```text
STATUS_LED HIGH → Relay command ON
STATUS_LED LOW  → Relay command OFF
```

---

## 6. MQTT Communication

### Subscribed Topic

The Arduino listens for relay commands on:

```text
greenhouse/actuator/relay/set
```

Expected command messages:

```text
ON
OFF
```

### Published Topic

The Arduino publishes its current relay status on:

```text
greenhouse/actuator/relay/status
```

Example status messages:

```text
ON
OFF
Arduino relay node connected
```

---

## 7. Software Functionality

The Arduino program performs these tasks:

1. Connects to WiFi using `WiFiNINA`.
2. Connects to the Raspberry Pi MQTT broker using `PubSubClient`.
3. Subscribes to the relay command topic.
4. Receives `ON` or `OFF` command messages.
5. Switches the KY-019 relay using active LOW logic.
6. Turns the onboard status LED ON/OFF.
7. Publishes the relay status back to MQTT.
8. Reconnects automatically if MQTT connection is lost.

---

## 8. Raspberry Pi MQTT Broker

The Raspberry Pi MQTT broker IP used in the Arduino code is:

```cpp
const char* mqtt_server = "172.20.10.4";
```

If the Raspberry Pi IP address changes, this value must be updated in the Arduino code.

To check the Raspberry Pi IP address:

```bash
hostname -I
```

---

## 9. Manual Test Commands

From the Raspberry Pi terminal, the relay can be tested manually.

### Turn relay ON

```bash
mosquitto_pub -t greenhouse/actuator/relay/set -m "ON"
```

Expected result:

```text
Relay clicks ON
Relay LED/actuator LED turns ON
Arduino serial monitor prints: Relay ON
```

### Turn relay OFF

```bash
mosquitto_pub -t greenhouse/actuator/relay/set -m "OFF"
```

Expected result:

```text
Relay clicks OFF
Relay LED/actuator LED turns OFF
Arduino serial monitor prints: Relay OFF
```

### Monitor relay status

```bash
mosquitto_sub -t greenhouse/actuator/relay/status -v
```

---

## 10. Automatic Control Test

The Raspberry Pi dashboard automatically sends relay commands when the temperature is compared with the regulator threshold.

Example test:

```bash
mosquitto_pub -t greenhouse/rotation -m "28"
mosquitto_pub -t greenhouse/temperature -m "25"
```

Expected result:

```text
Relay OFF
```

Now send higher temperature:

```bash
mosquitto_pub -t greenhouse/temperature -m "31"
```

Expected result:

```text
Relay ON
```

Now increase the threshold:

```bash
mosquitto_pub -t greenhouse/rotation -m "35"
```

Expected result:

```text
Relay OFF
```

---

## 11. Required Arduino Libraries

Install these libraries in Arduino IDE:

- `WiFiNINA`
- `PubSubClient`

The board must be selected as:

```text
Arduino Uno WiFi Rev2
```

---

## 12. Troubleshooting

### Relay LED works but relay output does not switch

Use active LOW logic:

```cpp
digitalWrite(RELAY_PIN, LOW);   // ON
digitalWrite(RELAY_PIN, HIGH);  // OFF
```

### MQTT command not received

Check if Arduino is connected to WiFi and MQTT. The serial monitor should show:

```text
WiFi connected
Connecting to MQTT...connected
```

### Relay does not react from dashboard

Make sure the Raspberry Pi dashboard publishes to:

```text
greenhouse/actuator/relay/set
```

not the old topic:

```text
greenhouse/pump
```

### Wrong relay behavior

Check if the LED is connected to the correct relay terminal:

```text
Use NO = Normally Open
Do not use NC unless opposite behavior is needed
```

---

## 13. Final Result

The Arduino relay node successfully receives MQTT commands from the Raspberry Pi and controls the KY-019 relay. The relay can be controlled manually from the dashboard or automatically by the Raspberry Pi based on the temperature and regulator threshold value.
