#include <Servo.h>

#include <WiFi.h>
#include <PubSubClient.h>

#define SERVO_PIN 23

// Credenciais da rede Wifi
const char* ssid = "";
const char* password = "";

// Configuración do broker MQTT
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "devices/NAPIoT-P2-Rec";

// Topics
const char* topic = "SDGC-NAPIoT/door/open/";

Servo myservo;
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);

void setup(){

  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  myservo.attach(SERVO_PIN);

}

void loop(){

  if (!client.connected())
    reconnect();

  client.loop();

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

    if (client.connect("NAPIoT-P2-Door")) {

      Serial.println("conectado!");
        client.subscribe(topic);
        client.setCallback(callback);


    } else {

      Serial.print("erro na conexión, erro=");
      Serial.print(client.state());
      Serial.println(" probando de novo en 5 segundos");
      delay(5000);

    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (int i = 0; i < length; i++)
    Serial.print((char)payload[i]);
  
  Serial.println();

  if ((char)payload[0] == '1')
    myservo.write(90);
  
  else
    myservo.write(0);

}
