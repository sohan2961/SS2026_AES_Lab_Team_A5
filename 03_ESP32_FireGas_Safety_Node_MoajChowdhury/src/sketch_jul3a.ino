// Include necessary libraries for Uno WiFi Rev2
#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>

// --- Configuration ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "192.168.x.x"; // Raspberry Pi IP

// --- Pin Definitions ---
const int gasPin = A0;      // Keyes Gas Sensor OUT
const int flamePin = A1;    // KY-026 Analog OUT
const int ledPin = 5;       // KY-034 Auto-flashing LED
const int buzzerPin = 8;    // KY-006 Passive Buzzer Signal

// --- Thresholds ---
const int GAS_THRESHOLD = 400;   
const int FLAME_THRESHOLD = 500; 

WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient);

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("SafetyNode_Uno")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void triggerAlarm(bool state) {
  if (state) {
    tone(buzzerPin, 1000); // 1kHz warning tone
    digitalWrite(ledPin, HIGH); // Trigger KY-034 flashing
  } else {
    noTone(buzzerPin);
    digitalWrite(ledPin, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  setup_wifi();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read analog values
  int gasValue = analogRead(gasPin);
  int flameValue = analogRead(flamePin); 

  // Debugging output
  Serial.print("Gas: "); Serial.print(gasValue);
  Serial.print(" | Flame: "); Serial.println(flameValue);

  // Evaluate Safety State
  bool gasDanger = (gasValue > GAS_THRESHOLD);
  // Note: Flame analog usually DROPS when fire is near
  bool flameDanger = (flameValue < FLAME_THRESHOLD); 
  bool alarmActive = gasDanger || flameDanger;

  triggerAlarm(alarmActive);

  // MQTT Payload Construction
  char gasStr[8], flameStr[8];
  itoa(gasValue, gasStr, 10);
  itoa(flameValue, flameStr, 10);

  // Publish to Team A5 Architecture Topics
  client.publish("greenhouse/safety/smoke", gasStr);
  client.publish("greenhouse/safety/flame", flameStr);
  
  if (alarmActive) {
    client.publish("greenhouse/safety/status", "DANGER");
  } else {
    client.publish("greenhouse/safety/status", "SAFE");
  }

  delay(2000); // Wait 2 seconds between readings
}