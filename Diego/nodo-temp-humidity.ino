#include <DHT.h>

#include <string.h>

#include <WiFi.h>
#include <PubSubClient.h>

#define DHT11_PIN 18

// Credenciais da rede Wifi
const char* ssid = "";
const char* password = "";

// Configuración do broker MQTT
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "devices/NAPIoT-P2-Rec";

// Topics
const char* topic_temp = "SDGC-NAPIoT/temp-hum/temperature";
const char* topic_hum = "SDGC-NAPIoT/temp-hum/humidity";

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht11(DHT11_PIN, DHT11);

char humC[4];
char tempC[4];
float temperature;
float humidity;

void setup_wifi();
void reconnect();

void setup() {
 
  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);

  dht11.begin();

}

void loop() {

  // Verifica se o cliente está conectado
  if (!client.connected()) {
    reconnect();
  }

  humidity = dht11.readHumidity();
  temperature = dht11.readTemperature();
  
  if (humidity)
    client.publish(topic_hum, dtostrf(humidity, 2, 2, humC));

  if (temperature)
    client.publish(topic_temp, dtostrf(temperature, 2, 2, tempC));

    sleep(5);
  
}

void setup_wifi() {

  delay(10);

  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectada!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {

    Serial.print("Intentando conectar a broker MQTT...");
    // Inténtase conectar indicando o ID do dispositivo
    //IMPORTANTE: este ID debe ser único!

    if (client.connect("NAPIoT-P2-Rec-TempHum")) {

      Serial.println("conectado!");

    } else {

      Serial.print("erro na conexión, erro=");
      Serial.print(client.state());
      Serial.println(" probando de novo en 5 segundos");
      delay(5000);
    }
  }
}
