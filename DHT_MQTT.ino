#include <ESP8266WiFi.h>

const char* ssid = "Mr. Blue Sky";
const char* password = "tanomodem";
WiFiClient nodemcuClient;

#include <PubSubClient.h>

const char* mqtt_Broker = "mqtt.eclipse.org";
const char* mqtt_ClientID = "termometro-hugohvf";
PubSubClient client(nodemcuClient);
const char* topicoTemperatura = "hugohvf/temp";
const char* topicoUmidade = "hugohvf/umid";

#include "DHT.h"
#define DHTPIN D3     
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

float h = dht.readHumidity();
float t = dht.readTemperature();

void setup() {
  Serial.begin(9600);
  conectarWifi();
  client.setServer(mqtt_Broker, 1883);
  dht.begin();
}

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  medir();
  publicar();

}

void conectarWifi() {
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void medir() {
  delay(2000);

  h = dht.readHumidity();
  t = dht.readTemperature();
  
  Serial.print(("Umidade: "));
  Serial.print(h);
  Serial.print(("%  Temperatura: "));
  Serial.print(t);
  Serial.println(("°C "));
}

void publicar() {
  client.publish(topicoTemperatura, String(t).c_str(), true);
  client.publish(topicoUmidade, String(h).c_str(), true);
}


void reconnect() {
  
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(mqtt_ClientID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
