#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "";
const char* password = "";

const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "devices/AirQuality";

WiFiClient esp_client;
PubSubClient client(esp_client);

int gas_pin = A4;
int value = 0;
char buffer[20];

void setup_wifi() 
{
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
}

void setup_mqtt_broker()
{
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("NAPIoT-P2-Rec-AirQuality")) 
    {
      Serial.println("Connected to MQTT broker.");
    }
    else
    {
      Serial.println("Error while trying to connect to MQTT broker.");
      delay(5000);
    }
  }
}

void setup() 
{
  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  setup_mqtt_broker();

  pinMode(gas_pin, INPUT);
}

void loop() 
{
  if (!client.connected())
  {
    setup_mqtt_broker();
  }

  value = analogRead(gas_pin);
  Serial.println(value);
  client.publish(mqtt_topic, itoa(value, buffer, 10));

  client.loop();
  
  delay(1000);
}