#include <WiFi.h>
#include <PubSubClient.h>

// Credenciais da rede Wifi
const char* ssid = "";     // Substituír polo SSID da nosa rede WiFi
const char* password = ""; // Substituír polo contrasinal da nosa rede WiFi

// Configuración do broker MQTT
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "SDCG-NAPIoT/soilMoisture";

// Constantes de calibración 
const int valorSeco = 0;       // Valor mínimo de humidade no solo
const int valorMollado = 2229; // Valor máximo de humidade no solo

WiFiClient espClient;
PubSubClient client(espClient);

const int sensorPin = 11; // pin do sensor de humidade do solo

// Función para conectar á WiFi
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

  Serial.println("");
  Serial.println("WiFi conectada!");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

// Reconecta co broker MQTT se se perde a conexión
void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conectar a broker MQTT...");

    // Inténtase conectar indicando o ID do dispositivo
    // IMPORTANTE: este ID debe ser único!
    if (client.connect("SDCG-NAPIoT/soilMoisture")) {
      Serial.println("conectado!");

      // Subscripción ao topic
      client.subscribe(mqtt_topic);
      Serial.println("Subscrito ao topic");
    } else {
      Serial.print("erro na conexión, erro=");
      Serial.print(client.state());
      Serial.println(" probando de novo en 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  // Configuración do sensor de humidade do solo
  pinMode(sensorPin, INPUT); // Configuramos o GPIO 11 como entrada
  // Configuración do porto serie
  Serial.begin(115200);
  // Conexión coa WiFi
  setup_wifi();
  // Configuración de MQTT
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {

  // Verifica se o cliente está conectado
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Lemos o valor analóxico
  int valorHumidade = analogRead(sensorPin);

  // Convertemos a porcentaxe usando map()
  int porcentaxe = map(valorHumidade, valorSeco, valorMollado, 0, 100);

  // Limitamos o rango (para que non baixe de 0 nin suba de 100 por ruido)
  porcentaxe = constrain(porcentaxe, 0, 100);

  // Enviamos o valor da porcentaxe de humidade do solo por MQTT
  char msg[10];
  snprintf(msg, 10, "%d", porcentaxe);

  // Mostramos por pantalla o valor de humidade no solo
  Serial.print("Humidade do solo (analóxico): ");
  Serial.print(valorHumidade);
  Serial.print("| Humidade do solo (%): ");
  Serial.print(porcentaxe);
  Serial.println("%");

  // Publica a mensaxe no tópico indicado
  client.publish(mqtt_topic, msg);

  // Esperar 5 segundos
  delay(5000);
}
