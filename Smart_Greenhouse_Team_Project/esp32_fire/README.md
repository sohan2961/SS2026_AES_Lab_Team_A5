# ESP32-2 — Fire and Gas Safety Node

**Assigned to:** Moaj Chowdhury  
**Node:** ESP32-2 — Smoke, Gas, Flame Detection + Buzzer + RGB LED

---

## Task Overview

You are responsible for the safety monitoring node. This ESP32 continuously monitors smoke, gas, and flame. When danger is detected, it immediately activates the buzzer and changes the RGB LED color to red. Alert status is published to the Raspberry Pi via MQTT.

---

## Hardware

| Component | Module | Connection |
|-----------|--------|------------|
| MQ-2 smoke/gas sensor | Analog + digital output | Analog pin (GPIO 32) |
| KY-026 analog flame sensor | Analog + digital output | Analog pin (GPIO 33) |
| KY-006 passive piezo buzzer | PWM buzzer | Digital GPIO pin (GPIO 25) |
| KY-016 RGB LED (5 mm) | Common cathode RGB | R → GPIO 27, G → GPIO 26, B → GPIO 14 |

> Use current-limiting resistors (220 Ω recommended) on each RGB LED pin.

---

## Software / Tools

- **Platform:** Arduino IDE or PlatformIO
- **Board:** ESP32 (select "ESP32 Dev Module")
- **Libraries:**
  - `PubSubClient` (MQTT)
  - `WiFi.h` (built-in)
  - No special library needed for MQ-2, KY-026, buzzer, or RGB LED

---

## Responsibilities

1. Wire MQ-2, KY-026, KY-006, and KY-016 to the ESP32
2. Read analog values from MQ-2 and KY-026 continuously
3. Define threshold values for smoke/gas and flame detection
4. When danger is detected:
   - Activate the KY-006 buzzer (PWM tone)
   - Switch the KY-016 RGB LED to **red**
5. When status is normal:
   - Keep the RGB LED **green**
   - Buzzer silent
6. Publish sensor readings and alarm state to the MQTT broker
7. Test alarm triggering and confirm alerts appear on the Raspberry Pi terminal

---

## MQTT Topics — Publish

```text
greenhouse/smoke    →  integer ADC value, e.g. "450"
greenhouse/flame    →  "0" (no flame) or "1" (flame detected)
greenhouse/alarm    →  "0" (safe) or "1" (danger)
```

---

## LED Status Logic

| Condition | RGB LED Color | Buzzer |
|-----------|--------------|--------|
| All clear | Green | Off |
| Smoke / gas above threshold | Red | On |
| Flame detected | Red | On |
| Both triggered | Red (flashing) | On |

---

## File Structure

```
esp32_fire/
├── README.md              ← this file
├── wiring_diagram.md      ← pin connections and wiring notes
└── esp32_fire_node/
    └── esp32_fire_node.ino ← main Arduino sketch
```

---

## Deliverables

- [ ] MQ-2 sensor reading smoke/gas analog values
- [ ] KY-026 detecting flame (test with a lighter — carefully)
- [ ] KY-006 buzzer activating correctly on alarm
- [ ] KY-016 LED showing green (safe) and red (alarm)
- [ ] MQTT messages visible in the Raspberry Pi monitoring terminal
- [ ] Threshold values documented in code comments
- [ ] Wiring diagram or photo of the assembled node

---

## Important Notes

- The MQ-2 sensor requires a **warm-up time of ~30 seconds** after power-on before readings are stable.
- Start with a **conservative threshold** (e.g. ADC > 400 for smoke) and adjust during testing.
- The KY-026 outputs both **analog** (continuous) and **digital** (threshold-triggered) signals — use the analog pin for more control.
- Get the **Raspberry Pi IP address** from Mostafizur to configure the MQTT broker address.
- Use **PWM `tone()`** for the buzzer to generate an audible frequency (e.g. 1000 Hz).
