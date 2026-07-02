"""
app.py — Greenhouse Central Controller
Raspberry Pi 3, Raspberry Pi OS 64-bit
Flask web dashboard + paho-mqtt subscriber + CSV logger
"""

import csv
import os
import threading
from datetime import datetime
from collections import deque

import paho.mqtt.client as mqtt
from flask import Flask, jsonify, render_template, request, send_file

# ── Configuration ─────────────────────────────────────────────────
BROKER_HOST = "localhost"
BROKER_PORT  = 1883
last_relay_command = None

TOPICS = [
    "greenhouse/temperature",
    "greenhouse/humidity",
    "greenhouse/smoke",
    "greenhouse/flame",
    "greenhouse/rotation",
    "greenhouse/alarm",
]

LOG_FILE = os.path.join(os.path.dirname(__file__), "logs", "sensor_log.csv")
os.makedirs(os.path.dirname(LOG_FILE), exist_ok=True)

# ── Shared state ───────────────────────────────────────────────────
state = {
    "temperature":      "--",
    "humidity":         "--",
    "smoke":            "--",
    "flame":            "0",
    "rotation":         "--",
    "alarm":            "0",
    "broker_connected": False,
    "last_update":      "--",
}

history = {
    "temperature": deque(maxlen=60),
    "humidity":    deque(maxlen=60),
    "smoke":       deque(maxlen=60),
}

recent_log = deque(maxlen=100)

# ── CSV logging ────────────────────────────────────────────────────
def write_csv(timestamp, topic, value):
    file_exists = os.path.isfile(LOG_FILE)
    with open(LOG_FILE, "a", newline="") as f:
        writer = csv.writer(f)
        if not file_exists:
            writer.writerow(["timestamp", "topic", "value"])
        writer.writerow([timestamp, topic, value])

# ── MQTT callbacks ─────────────────────────────────────────────────
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        state["broker_connected"] = True
        print("[MQTT] Connected to broker")
        for topic in TOPICS:
            client.subscribe(topic)
            print(f"  subscribed → {topic}")
    else:
        state["broker_connected"] = False
        print(f"[MQTT] Connection failed rc={rc}")

def on_disconnect(client, userdata, rc):
    state["broker_connected"] = False
    print("[MQTT] Disconnected")

def auto_control_relay():
    global last_relay_command

    try:
        temperature = float(state["temperature"])
        threshold = float(state["rotation"])
    except ValueError:
        print("[AUTO] Waiting for valid temperature and threshold...")
        return

    if temperature > threshold:
        command = "ON"
    else:
        command = "OFF"

    if command != last_relay_command:
        mqtt_client.publish("greenhouse/actuator/relay/set", command)
        last_relay_command = command
        print(f"[AUTO] Temperature={temperature}°C, Threshold={threshold}°C → Relay {command}")

def on_message(client, userdata, msg):
    topic = msg.topic
    value = msg.payload.decode().strip()
    ts    = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    key = topic.split("/")[-1]
    if key in state:
        state[key]           = value
        state["last_update"] = ts

    if key in ("temperature", "rotation"):
        auto_control_relay()

    if key in history:
        try:
            history[key].append({
                "time":  datetime.now().strftime("%H:%M:%S"),
                "value": float(value),
            })
        except ValueError:
            pass

    recent_log.appendleft({
        "ts":    ts,
        "topic": topic,
        "value": value,
        "key":   key,
    })

    write_csv(ts, topic, value)
    print(f"[{ts}]  {topic:<35} {value}")

# ── MQTT client ────────────────────────────────────────────────────
mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1)
mqtt_client.on_connect    = on_connect
mqtt_client.on_disconnect = on_disconnect
mqtt_client.on_message    = on_message

def start_mqtt():
    import time
    while True:
        try:
            mqtt_client.connect(BROKER_HOST, BROKER_PORT, keepalive=60)
            mqtt_client.loop_forever()
        except Exception as e:
            print(f"[MQTT] Reconnecting… ({e})")
            time.sleep(5)

mqtt_thread = threading.Thread(target=start_mqtt, daemon=True)
mqtt_thread.start()

# ── Flask app ──────────────────────────────────────────────────────
app = Flask(__name__)

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/api/state")
def api_state():
    return jsonify({
        **state,
        "history": {k: list(v) for k, v in history.items()},
        "log":     list(recent_log)[:20],
    })

@app.route("/api/pump", methods=["POST"])
def api_pump():
    data    = request.get_json(force=True)
    command = data.get("command", "OFF").upper()
    if command not in ("ON", "OFF"):
        return jsonify({"error": "Invalid command"}), 400
    mqtt_client.publish("greenhouse/actuator/relay/set", command)
    ts = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    recent_log.appendleft({
        "ts":    ts,
        "topic": "greenhouse/actuator/relay/set",
        "value": command,
        "key":   "pump",
    })
    print(f"[PUMP] Command sent → {command}")
    return jsonify({"status": "ok", "command": command})

@app.route("/api/log/download")
def download_log():
    if not os.path.isfile(LOG_FILE):
        return "No log file yet.", 404
    return send_file(LOG_FILE, mimetype="text/csv",
                     download_name="sensor_log.csv", as_attachment=True)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=False)
