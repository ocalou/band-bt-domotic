#include <DHT.h>

#include <string.h>

#include <WiFi.h>
#include <PubSubClient.h>

#define DHT11_PIN 18
#define AIR_PIN A5

// Credenciais da rede Wifi
const char* ssid = "";
const char* password = "";

// Configuración do broker MQTT
const int mqtt_port = 1883;

// Topics
const char* topic_temp = "sdgc/cocina/temperatura";
const char* topic_hum = "sdgc/cocina/humedad";
const char* topic_aire = "sdgc/cocina/calidad-aire";

// IP da Raspberry Pi 5 (Dispositivo Fog) 
const char* fog_server = "";

// IP do Cloudlet
const char* cloudlet_server = ""; 

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht11(DHT11_PIN, DHT11);

char humC[4];
char tempC[4];
char aire[4];
char rssi[4];
float temperature;
float humidity;
float air_quality;

void setup_wifi();
void reconnect();


void setup() {
 
  Serial.begin(115200);

  setup_wifi();

  dht11.begin();
}

void onReadValue() {
  onReadValueDHT11();
  onReadValueAirQuality();

  delay(500);
}

void onReadValueDHT11(){

  humidity = 0;
  temperature = 0;

  for (int i = 0; i < 10; i++){
    humidity += dht11.readHumidity();
    temperature += dht11.readTemperature();
    delay(50);
  }

  humidity /= 10;
  temperature /= 10;

  if (humidity)
    client.publish(topic_hum, dtostrf(humidity, 2, 2, humC));

  if (temperature)
    client.publish(topic_temp, dtostrf(temperature, 2, 2, tempC));
    
}

void onReadValueAirQuality() {
  air_quality = analogRead(AIR_PIN);

  delay(50);

  client.publish(topic_aire, itoa(air_quality, aire, 10));
}

void loop() {
  
  client.loop();

  if (!client.connected()) {
    reconnect();
  }

  onReadValue();

}

void setup_wifi() {

  Serial.println("Intentando conectar ao AP");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Error ao conectar ao AP");
    delay(1000);
  }

}

void reconnect() {
  while (!client.connected()) {
    
    Serial.print("Intentando conectar a FOG (Raspberry Pi ");
    Serial.print(fog_server);
    Serial.print(")...");
    
    client.setServer(fog_server, mqtt_port);
    
    if (client.connect("NodoCocina")) {
      Serial.println("Conectado a Fog");
      return;
    } else {
      Serial.print(" erro, rc=");
      Serial.print(client.state());
      Serial.println(". Probando Cloudlet...");
    }

    Serial.print("Intentando conectar a CLOUDLET (PC ");
    Serial.print(cloudlet_server);
    Serial.print(")...");

    client.setServer(cloudlet_server, mqtt_port);

    if (client.connect("NodoCocina")) {
      Serial.println("Conectado a Cloudlet");
      return;
    } else {
      Serial.print(" erro, rc=");
      Serial.print(client.state());
      
      Serial.println(". Todos os servidores fallaron. Reintentando en 5 segundos...");
      delay(5000);
    }
  }
}