#include <WiFiNINA.h>
#include <PubSubClient.h>

#define RELAY_PIN 7
#define STATUS_LED 13

// Change these
const char* ssid = "Sohan";
const char* password = "SOap5689";

// Raspberry Pi MQTT broker IP
const char* mqtt_server = "172.20.10.4";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setRelay(bool state) {
  if (state) {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(STATUS_LED, HIGH);
    client.publish("greenhouse/actuator/relay/status", "ON");
    Serial.println("Relay ON");
  } else {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(STATUS_LED, LOW);
    client.publish("greenhouse/actuator/relay/status", "OFF");
    Serial.println("Relay OFF");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("MQTT message received: ");
  Serial.print(topic);
  Serial.print(" -> ");
  Serial.println(message);

  if (String(topic) == "greenhouse/actuator/relay/set") {
    if (message == "ON") {
      setRelay(true);
    } 
    else if (message == "OFF") {
      setRelay(false);
    }
  }
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    Serial.print(".");
    delay(3000);
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("Arduino IP: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");

    if (client.connect("Arduino_Relay_Node_Deepak")) {
      Serial.println("connected");

      client.subscribe("greenhouse/actuator/relay/set");
      client.publish("greenhouse/actuator/relay/status", "Arduino relay node connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 3 seconds");
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  // Safe default: relay OFF at startup
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(STATUS_LED, LOW);

  connectWiFi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }

  client.loop();
}