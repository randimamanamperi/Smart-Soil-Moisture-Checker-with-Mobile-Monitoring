#include <WiFi.h>
#include <PubSubClient.h>

#define PUMP_LED_PIN 5
const int moistureThreshold = 300; // Adjust as needed

const char* ssid = "vv";
const char* password = "vvvvvvvv";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "esp32/soilSensorData";

WiFiClient espClient;
PubSubClient client(espClient);

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
    if (client.connect("ESP32_CommNode")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");

  String messageTemp;
  for (unsigned int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }
  Serial.println(messageTemp);

  int moisture = -1;
  float temperature = -1.0;
  float humidity = -1.0;

  int mIndex = messageTemp.indexOf("moisture_level");
  if (mIndex != -1) {
    int start = messageTemp.indexOf(':', mIndex) + 1;
    int end = messageTemp.indexOf(',', mIndex);
    if (end == -1) end = messageTemp.indexOf('}', mIndex);
    moisture = messageTemp.substring(start, end).toInt();
  }

  int tIndex = messageTemp.indexOf("temperature");
  if (tIndex != -1) {
    int start = messageTemp.indexOf(':', tIndex) + 1;
    int end = messageTemp.indexOf(',', tIndex);
    if (end == -1) end = messageTemp.indexOf('}', tIndex);
    temperature = messageTemp.substring(start, end).toFloat();
  }

  int hIndex = messageTemp.indexOf("humidity");
  if (hIndex != -1) {
    int start = messageTemp.indexOf(':', hIndex) + 1;
    int end = messageTemp.indexOf(',', hIndex);
    if (end == -1) end = messageTemp.indexOf('}', hIndex);
    humidity = messageTemp.substring(start, end).toFloat();
  }

  if (moisture != -1 && temperature != -1.0 && humidity != -1.0) {
    Serial.printf("Parsed data: Moisture=%d, Temperature=%.2f°C, Humidity=%.2f%%\n", moisture, temperature, humidity);

    if (moisture < moistureThreshold) {
      digitalWrite(PUMP_LED_PIN, HIGH);
      Serial.println("Soil dry - Pump ON");
    } else {
      digitalWrite(PUMP_LED_PIN, LOW);
      Serial.println("Soil moist - Pump OFF");
    }
  } else {
    Serial.println("Failed to parse sensor data.");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PUMP_LED_PIN, OUTPUT);
  digitalWrite(PUMP_LED_PIN, LOW);

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  yield(); // Prevent watchdog reset
}

