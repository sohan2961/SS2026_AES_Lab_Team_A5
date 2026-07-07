#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>

const char* ssid = "Sohan";
const char* password = "SOap5689";

const char* mqtt_server = "172.20.10.5";


const int gasPin = A0;       // Keyes Gas Sensor OUT
const int flamePin = A1;     // KY-026 Analog OUT
const int ledPin = 5;        // KY-034 Auto-flashing LED
const int buzzerPin = 8;     // KY-006 Passive Buzzer Signal


const int GAS_THRESHOLD = 400;
const int FLAME_THRESHOLD = 500;


WiFiClient wifiClient;
PubSubClient client(wifiClient);


void printWifiStatusCode(int status) {
  Serial.print("WiFi status code: ");
  Serial.print(status);
  Serial.print(" -> ");

  switch (status) {
    case WL_IDLE_STATUS:
      Serial.println("IDLE");
      break;

    case WL_NO_SSID_AVAIL:
      Serial.println("SSID not found");
      break;

    case WL_CONNECTED:
      Serial.println("CONNECTED");
      break;

    case WL_CONNECT_FAILED:
      Serial.println("Connection failed, check password");
      break;

    case WL_CONNECTION_LOST:
      Serial.println("Connection lost");
      break;

    case WL_DISCONNECTED:
      Serial.println("Disconnected");
      break;

    default:
      Serial.println("Unknown status");
      break;
  }
}


void setup_wifi() {
  delay(1000);

  Serial.println();
  Serial.println("=========================================");
  Serial.println("Checking WiFi module...");
  Serial.println("=========================================");

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("[ERROR] WiFi module not detected!");
    Serial.println("Check if you selected the correct board: Arduino Uno WiFi Rev2");
    while (true);
  }

  Serial.print("WiFiNINA firmware version: ");
  Serial.println(WiFi.firmwareVersion());

  Serial.println();
  Serial.print("Connecting to WiFi Network: ");
  Serial.println(ssid);

  WiFi.disconnect();
  delay(1000);

  int status = WL_IDLE_STATUS;
  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    Serial.print("Attempt ");
    Serial.print(attempts + 1);
    Serial.println(" to connect...");

    status = WiFi.begin(ssid, password);

    delay(3000);

    status = WiFi.status();
    printWifiStatusCode(status);

    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("=========================================");
    Serial.println("[SUCCESS] WiFi Connection Established!");
    Serial.print("Arduino IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength RSSI: ");
    Serial.println(WiFi.RSSI());
    Serial.println("=========================================");
  } else {
    Serial.println();
    Serial.println("=========================================");
    Serial.println("[FAILED] Could not connect to WiFi.");
    Serial.println("Check these:");
    Serial.println("1. SSID and password are correct");
    Serial.println("2. WiFi is 2.4 GHz, not 5 GHz");
    Serial.println("3. Phone hotspot compatibility mode is ON");
    Serial.println("4. Arduino Uno WiFi Rev2 board is selected");
    Serial.println("5. WiFiNINA firmware is updated");
    Serial.println("=========================================");

    while (true);
  }
}

// ================= MQTT RECONNECT =================
void reconnect() {
  while (!client.connected()) {
    Serial.println();
    Serial.print("Connecting to MQTT Broker at ");
    Serial.print(mqtt_server);
    Serial.println("...");

    if (client.connect("SafetyNode_Uno")) {
      Serial.println("=========================================");
      Serial.println("[SUCCESS] MQTT Broker Connection Active!");
      Serial.println("Node is now broadcasting safety data.");
      Serial.println("=========================================");
    } else {
      Serial.print("[ERROR] MQTT Connection Failed, rc=");
      Serial.print(client.state());
      Serial.println(" -> Retrying in 5 seconds...");

      delay(5000);
    }
  }
}


void triggerAlarm(bool state) {
  if (state) {
    tone(buzzerPin, 1000);
    digitalWrite(ledPin, HIGH);
  } else {
    noTone(buzzerPin);
    digitalWrite(ledPin, LOW);
  }
}


void setup() {
  Serial.begin(115200);

  delay(2000);

  Serial.println();
  Serial.println("System Booting...");
  Serial.println("Initializing Safety Node Hardware.");

  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, LOW);
  noTone(buzzerPin);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
}


void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WARNING] WiFi disconnected. Reconnecting...");
    setup_wifi();
  }

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  // Read analog sensor values
  int gasValue = analogRead(gasPin);
  int flameValue = analogRead(flamePin);

  // Print real-time diagnostics
  Serial.print("[DATA] Gas Level: ");
  Serial.print(gasValue);
  Serial.print(" | Flame Level: ");
  Serial.println(flameValue);

  // Evaluate safety state
  bool gasDanger = gasValue > GAS_THRESHOLD;

  // For KY-026 flame sensor, usually lower analog value means stronger flame
  bool flameDanger = flameValue < FLAME_THRESHOLD;

  bool alarmActive = gasDanger || flameDanger;

  triggerAlarm(alarmActive);

  // Convert sensor values to text for MQTT
  char gasStr[8];
  char flameStr[8];

  itoa(gasValue, gasStr, 10);
  itoa(flameValue, flameStr, 10);

  // Publish MQTT sensor data
  client.publish("greenhouse/safety/smoke", gasStr);
  client.publish("greenhouse/safety/flame", flameStr);

  if (alarmActive) {
    client.publish("greenhouse/safety/status", "DANGER");
    Serial.println("[ALERT] Critical levels detected! Publishing DANGER status.");
  } else {
    client.publish("greenhouse/safety/status", "SAFE");
    Serial.println("[OK] Safety status is SAFE.");
  }

  delay(2000);
}