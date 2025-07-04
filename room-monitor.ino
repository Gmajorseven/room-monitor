#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "secrets.h"
const char* ssid = SECRET_SSID; 
const char* password = SECRET_PASS;
const char* mqtt_server = SECRET_MQTT_SERVER;
const int mqtt_port = SECRET_MQTT_PORT; // 1883 for non-SSL, 8883 for SSL/TLS
const char* mqtt_Client = SECRET_MQTT_CLIENT;
const char* mqtt_username = SECRET_MQTT_USER; // If applicable, otherwise set to NULL
const char* mqtt_password = SECRET_MQTT_PASS; // If applicable, otherwise set to NULL
float temperature, humidity = 0;
char msg[100];
unsigned long lastUpdateTime, currentMillis = 0;
long lastMsg = 0;
float lastTemperature = 0;
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(27, DHT22);
float readTemperature() {
  float t = dht.readTemperature();
  if(isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    return t;
  }
}
float readHumidity() {
  float h = dht.readHumidity();
  if(isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    return h;
  }
}
void reconnect() {
  while(!client.connected()) {
    Serial.print("Attempting NETPIE2020 connection…");
    if(client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
      Serial.println("NETPIE2020 connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}
void setup() {
  dht.begin();
  Serial.begin(115200);
  Serial.println("Starting...");
  if(WiFi.begin(ssid, password)) {
    while(WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, mqtt_port);
}
void loop() {
  if(!client.connected()) {
    reconnect();
  }
 
  currentMillis = millis();
  client.loop();

  temperature = readTemperature();
  humidity = readHumidity();
  String place = "Teddy's Room"; // You can change this to your desired place name

  if(currentMillis > 604800000) {
    ESP.restart();
  }

  if(abs(temperature - lastTemperature) >= 0.5 || abs(temperature - lastTemperature) >= 0.4) {
    lastUpdateTime = currentMillis;
    lastTemperature = temperature;
  }
  
  long now = millis();
  if(now - lastMsg > 2000) {
    lastMsg = now;
    String data = "{\"data\": {\"temp\":" + String(temperature) + ", \"humi\":" + String(humidity) + ", \"place\": \"" +  String(place) + "\", \"lastupdatetime\":" + String(lastUpdateTime) + "}}";
    Serial.println(data);
    Serial.print("Lastes Temperature: ");
    Serial.println(lastTemperature);
    Serial.print("Current Millisec: ");
    Serial.println(currentMillis);
    data.toCharArray(msg, (data.length() + 1));
    client.publish("@shadow/data/update", msg);
  }
  delay(1);
}
