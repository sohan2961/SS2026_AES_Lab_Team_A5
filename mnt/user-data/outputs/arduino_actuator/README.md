# Arduino Uno WiFi Rev2 — Actuator / Relay Node

**Assigned to:** Deepak Kapil  
**Node:** Actuator Control — Relay simulation via KY-019

---

## Task Overview

You are responsible for the actuator control node. The Arduino Uno WiFi Rev2 connects to the Wi-Fi network, subscribes to the relay command topic on the MQTT broker, and switches the KY-019 relay ON or OFF based on commands received from the Raspberry Pi. Since a real water pump is not available, the relay output currently controls a status LED to simulate the actuator.

---

## Hardware

| Component | Module | Connection |
|-----------|--------|------------|
| KY-019 5V relay module | Single-channel relay | Digital pin D7 |
| Status LED (simulation) | Standard LED + 220 Ω resistor | Relay NO terminal |
| Wi-Fi (built-in) | Arduino Uno WiFi Rev2 | Used for MQTT connection |

> **Relay wiring:** Connect the relay COM pin to 5V. Connect NO (normally open) to the LED anode. LED cathode → 220 Ω resistor → GND. When relay is ON, the circuit closes and the LED lights up.

---

## Software / Tools

- **Platform:** Arduino IDE
- **Board:** Arduino Uno WiFi Rev2 (`megaAVR` boards package)
- **Libraries:**
  - `WiFiNINA` (built-in with the board package)
  - `PubSubClient` (MQTT) — install via Library Manager

> **Important:** The Arduino Uno WiFi Rev2 has limited flash memory. Keep the sketch lean. Do not install ArduinoBLE at the same time as PubSubClient — they may not fit together.

---

## Responsibilities

1. Wire the KY-019 relay and status LED to the Arduino
2. Write an Arduino sketch to:
   - Connect to the Wi-Fi network
   - Connect to the Mosquitto MQTT broker on the Raspberry Pi
   - Subscribe to `greenhouse/pump`
   - Switch the relay ON when payload is `"ON"`
   - Switch the relay OFF when payload is `"OFF"`
3. Add a status LED on-board (pin 13) to indicate Wi-Fi and MQTT connection status
4. Test relay switching by sending manual MQTT commands from the Raspberry Pi terminal
5. Confirm relay response is visible in the Raspberry Pi monitoring log

---

## MQTT Topics — Subscribe

```text
greenhouse/pump    →  "ON"  → relay closes (LED on)
                   →  "OFF" → relay opens (LED off)
```

---

## Relay State Logic

| MQTT Payload | Relay State | Status LED (pin 13) |
|-------------|-------------|-------------------|
| `"ON"` | Closed (active) | On |
| `"OFF"` | Open (inactive) | Off |
| No connection | Open (safe default) | Blinking |

---

## File Structure

```
arduino_actuator/
├── README.md                    ← this file
├── wiring_diagram.md            ← pin connections and wiring notes
└── arduino_actuator_node/
    └── arduino_actuator_node.ino ← main Arduino sketch
```

---

## Deliverables

- [ ] KY-019 relay wired correctly with status LED
- [ ] Arduino connecting to Wi-Fi and MQTT broker successfully
- [ ] Relay switching ON/OFF in response to MQTT commands
- [ ] LED simulation confirming actuator state visually
- [ ] Relay command test documented (manual `mosquitto_pub` test from Pi terminal)
- [ ] Wiring diagram or photo of the assembled node

---

## Important Notes

- Get the **Raspberry Pi IP address** from Mostafizur and the **Wi-Fi credentials** from the team.
- The relay coil is driven at **5 V** — the Arduino Uno WiFi Rev2 GPIO outputs 5 V, so no level shifter is needed.
- Always set the relay pin to **LOW (off)** in `setup()` so the relay starts in a safe open state.
- In a future version, the relay NO terminal can be wired to a real **water pump or fan** instead of the LED.
- The `PubSubClient` library requires calling `client.loop()` inside `loop()` to keep the MQTT connection alive.
