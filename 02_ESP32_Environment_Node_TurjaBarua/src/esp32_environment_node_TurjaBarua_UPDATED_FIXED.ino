/*
  Smart Greenhouse Monitoring System
  Turja Barua Part: ESP32 Environment Monitoring Node

  UPDATED FIXED VERSION
  - Shows temperature and humidity on I2C LCD
  - Sends same data to Raspberry Pi dashboard through MQTT
  - MQTT reconnect is non-blocking, so LCD keeps working even if MQTT is offline
  - LCD address: 0x27
  - KY-015/DHT11 data pin: GPIO 4
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------------- User configuration ----------------
// Change Wi-Fi name and password before uploading.
const char* WIFI_SSID     = "Sohan";
const char* WIFI_PASSWORD = "SOap5689";

// Raspberry Pi MQTT broker IP address.
// From your Raspberry Pi dashboard, this is most likely 172.20.10.4.
const char* MQTT_SERVER   = "172.20.10.4";
const uint16_t MQTT_PORT  = 1883;

// ---------------- ESP32 pin configuration ----------------
#define DHT_PIN       4       // KY-015/DHT11 DATA pin connected to ESP32 GPIO 4
#define DHT_TYPE      DHT11   // KY-015 usually uses DHT11
#define ROTATION_PIN  34      // Rotation sensor signal pin
#define I2C_SDA       21      // LCD SDA
#define I2C_SCL       22      // LCD SCL
#define STATUS_LED    2       // ESP32 onboard LED on many boards

// ---------------- LCD configuration ----------------
#define LCD_ADDRESS   0x27    // Your I2C scanner found 0x27
#define LCD_COLUMNS   16
#define LCD_ROWS      2

// ---------------- Timing configuration ----------------
const unsigned long SENSOR_INTERVAL_MS = 3000;  // LCD + MQTT update every 3 seconds
const unsigned long MQTT_RETRY_MS      = 5000;  // Retry MQTT every 5 seconds

// ---------------- Threshold configuration ----------------
const int THRESHOLD_MIN_C = 20;
const int THRESHOLD_MAX_C = 45;

// ---------------- Objects ----------------
WiFiClient espClient;
PubSubClient mqttClient(espClient);
DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

unsigned long lastSensorTime = 0;
unsigned long lastMqttTry    = 0;

// ---------------- LCD helper ----------------
void lcdMessage(const String& line1, const String& line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1.substring(0, LCD_COLUMNS));
  lcd.setCursor(0, 1);
  lcd.print(line2.substring(0, LCD_COLUMNS));
}

// ---------------- Wi-Fi connection ----------------
void connectWiFi() {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);

  lcdMessage("WiFi connecting", WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
  }

  digitalWrite(STATUS_LED, HIGH);
  Serial.println();
  Serial.print("Wi-Fi connected. ESP32 IP: ");
  Serial.println(WiFi.localIP());

  lcdMessage("WiFi connected", WiFi.localIP().toString());
  delay(1200);
}

// ---------------- Non-blocking MQTT connection ----------------
void tryConnectMQTT() {
  if (mqttClient.connected()) {
    return;
  }

  unsigned long now = millis();
  if (now - lastMqttTry < MQTT_RETRY_MS) {
    return;
  }
  lastMqttTry = now;

  Serial.print("Connecting to MQTT broker: ");
  Serial.println(MQTT_SERVER);

  String clientId = "ESP32_ENV_Turja_" + String((uint32_t)ESP.getEfuseMac(), HEX);

  if (mqttClient.connect(clientId.c_str())) {
    Serial.println("MQTT connected.");
    mqttClient.publish("greenhouse/env/status", "ESP32_ENV_ONLINE", true);
    mqttClient.publish("greenhouse/env/ip", WiFi.localIP().toString().c_str(), true);
  } else {
    Serial.print("MQTT failed, rc=");
    Serial.println(mqttClient.state());
    // Important: do not block here. LCD must still update.
  }
}

// ---------------- Sensor helper ----------------
int readThresholdC() {
  int raw = analogRead(ROTATION_PIN);  // ESP32 ADC range: 0 to 4095
  int threshold = map(raw, 0, 4095, THRESHOLD_MIN_C, THRESHOLD_MAX_C);
  return constrain(threshold, THRESHOLD_MIN_C, THRESHOLD_MAX_C);
}

// ---------------- MQTT publish helpers ----------------
void publishFloat(const char* topic, float value, bool retained = true) {
  if (!mqttClient.connected()) {
    return;
  }

  char payload[16];
  dtostrf(value, 0, 1, payload);
  mqttClient.publish(topic, payload, retained);

  Serial.print(topic);
  Serial.print(" = ");
  Serial.println(payload);
}

void publishInt(const char* topic, int value, bool retained = true) {
  if (!mqttClient.connected()) {
    return;
  }

  char payload[12];
  snprintf(payload, sizeof(payload), "%d", value);
  mqttClient.publish(topic, payload, retained);

  Serial.print(topic);
  Serial.print(" = ");
  Serial.println(payload);
}

void publishText(const char* topic, const char* value, bool retained = true) {
  if (!mqttClient.connected()) {
    return;
  }

  mqttClient.publish(topic, value, retained);
}

// ---------------- Main read/display/publish function ----------------
void readDisplayAndPublish() {
  float temperature = dht.readTemperature();
  float humidity    = dht.readHumidity();
  int thresholdC    = readThresholdC();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT read failed. Check KY-015 wiring.");
    lcdMessage("DHT read error", "Check KY-015");
    publishText("greenhouse/env/status", "DHT_READ_ERROR", true);
    return;
  }

  bool tempHigh = temperature > thresholdC;
  const char* statusText = tempHigh ? "TEMP_HIGH" : "NORMAL";

  // 1. Show same data on LCD display.
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print((char)223);
  lcd.print("C H:");
  lcd.print(humidity, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Set:");
  lcd.print(thresholdC);
  lcd.print((char)223);
  lcd.print("C ");
  lcd.print(statusText);

  // 2. Send same data to Raspberry Pi dashboard through MQTT.
  if (mqttClient.connected()) {
    // Topics used by your Flask dashboard.
    publishFloat("greenhouse/temperature", temperature);
    publishFloat("greenhouse/humidity", humidity);
    publishInt("greenhouse/rotation", thresholdC);

    // Extra structured project topics.
    publishFloat("greenhouse/env/temperature", temperature);
    publishFloat("greenhouse/env/humidity", humidity);
    publishInt("greenhouse/env/threshold", thresholdC);
    publishText("greenhouse/env/status", statusText, true);
  }

  Serial.print("LCD updated: T=");
  Serial.print(temperature, 1);
  Serial.print(" C, H=");
  Serial.print(humidity, 1);
  Serial.print(" %, Threshold=");
  Serial.print(thresholdC);
  Serial.print(" C, MQTT=");
  Serial.println(mqttClient.connected() ? "ONLINE" : "OFFLINE");

  digitalWrite(STATUS_LED, tempHigh ? !digitalRead(STATUS_LED) : HIGH);
}

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();
  lcdMessage("Turja Barua", "ESP32 Env Node");
  delay(1500);

  dht.begin();

  connectWiFi();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

  // Try MQTT once, but do not block the LCD/sensor part.
  tryConnectMQTT();
}

// ---------------- Main loop ----------------
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(STATUS_LED, LOW);
    connectWiFi();
  }

  tryConnectMQTT();

  if (mqttClient.connected()) {
    mqttClient.loop();
  }

  unsigned long now = millis();
  if (now - lastSensorTime >= SENSOR_INTERVAL_MS) {
    lastSensorTime = now;
    readDisplayAndPublish();
  }
}
