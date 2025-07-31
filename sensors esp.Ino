#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// Pins and sensor type
#define MOISTURE_PIN 34
#define DHT_PIN 4
#define DHT_TYPE DHT11

// WiFi credentials - update to your network
const char* ssid = "vv";
const char* password = "vvvvvvvv";

// MQTT broker info
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "esp32/soilSensorData";

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHT_PIN, DHT_TYPE);

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_SensorNode")) {
      Serial.println("connected");
      // No subscriptions on Sensor Node
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

// Clamp moisture reading to ADC range
int measurementLimit(int val) {
  if (val < 0) return 0;
  if (val > 4095) return 4095;
  return val;
}

void setup() {
  Serial.begin(115200);
  pinMode(MOISTURE_PIN, INPUT);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int moisture = analogRead(MOISTURE_PIN);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  Serial.printf("Soil Moisture: %d, Temperature: %.2f°C, Humidity: %.2f%%\n", moisture, temperature, humidity);

  String payload = "{";
  payload += "\"moisture_level\":" + String(measurementLimit(moisture)) + ",";
  payload += "\"temperature\":" + String(temperature, 2) + ",";
  payload += "\"humidity\":" + String(humidity, 2);
  payload += "}";

  if (client.publish(mqtt_topic, payload.c_str())) {
    Serial.println("MQTT publish success");
  } else {
    Serial.println("MQTT publish failed");
  }

  // Delay 30 seconds with yield to feed watchdog
  for (int i = 0; i < 10; i++) {
    delay(1000);
    yield();
  }
}
