#include <PubSubClient.h>
#include <WiFi.h>

const char* ssid = "NAP_AP";      // Substituír polo SSID da nosa rede WiFi
const char* password = "NAPIOT_AP";  // Substituír polo password da nosa rede WiFi
const char* mqttServer = "test.mosquitto.org";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";

WiFiClient espClient;
PubSubClient client(espClient);

const int sensorPin = 3;  // Pin do sensor
const int ledPin = 14;    // Pin do LED
const int threshold = 200; // Threshold

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.println("...................................");
    Serial.print("Conectando á WiFi.");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connectado á rede WiFi!");
    client.setServer(mqttServer, mqttPort);
    while (!client.connected()) {
        Serial.println("Conectando ao broker MQTT...");
        // IMPORTANTE: O ID de cliente da seguinte línea (NodoNAPIoT) debe ser
        // “único”!!
        if (client.connect("NodoNAPIoT", mqttUser, mqttPassword))
            Serial.println("Conectado ao broker MQTT!");
        else {
            Serial.print("Erro ao conectar co broker: ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    pinMode(ledPin, OUTPUT);  // Poñémo-lo pin do LED en OUTPUT

}
void loop() {
    client.loop();
      
    int sensorValue = analogRead(sensorPin);  // Lemo-lo valor

    char str[16];
    sprintf(str, "%u", sensorValue);  // Con esto simulamos a xeración do valor dun sensor
    client.publish("SDGC-NAPIoT/luminosity2", str);  // Usar o mesmo topic que en Node-RED
    Serial.println(str);

    // Condición
    if (sensorValue > threshold) {
        digitalWrite(ledPin, LOW);  // Apagar LED
    } else {
        digitalWrite(ledPin, HIGH);   // Acender LED
    }

    delay(5000);
}
