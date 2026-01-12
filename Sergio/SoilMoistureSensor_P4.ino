#include <WiFi.h>
#include <PubSubClient.h>

// Credenciais da rede Wifi
const char* ssid = "NAP_AP"; 
const char* password = "NAPIOT_AP";

// --- CONFIGURACIÓN DE SERVIDORES 
// IP da Raspberry Pi 5 (Dispositivo Fog) 
const char* fog_server = "172.20.10.9"; 

// IP do Cloudlet
const char* cloudlet_server = "172.20.10.12"; 

const int mqtt_port = 1883;
const char* mqtt_topic = "sdgc/jardin/humedad";


// Constantes de calibración 
const int valorSeco = 0;      // Valor mínimo de humidade no solo
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

// LÓGICA MODIFICADA PARA FOG -> CLOUDLET
void reconnect() {
  // Bucle mentres non esteamos conectados
  while (!client.connected()) {
    
    // 1. INTENTO PRIMARIO: Conectar ao Dispositivo Fog (Raspberry Pi)
    Serial.print("Intentando conectar a FOG (Raspberry Pi at ");
    Serial.print(fog_server);
    Serial.print(")...");
    
    // Configuramos a IP do Fog
    client.setServer(fog_server, mqtt_port);
    
    if (client.connect("SoilSensorID")) { // Usar un ID único
      Serial.println(" CONECTADO a Fog!");
      client.subscribe(mqtt_topic);
      return; // Saímos da función se conectamos con éxito
    } else {
      Serial.print(" erro, rc=");
      Serial.print(client.state());
      Serial.println(". Probando Cloudlet...");
    }

    // 2. INTENTO SECUNDARIO (FAILOVER): Conectar ao Cloudlet
    Serial.print("Intentando conectar a CLOUDLET (PC at ");
    Serial.print(cloudlet_server);
    Serial.print(")...");

    // Cambiamos a IP da Raspberry Pi polo Cloudlet
    client.setServer(cloudlet_server, mqtt_port);

    if (client.connect("SoilSensorID")) {
      Serial.println(" CONECTADO a Cloudlet!");
      client.subscribe(mqtt_topic);
      return; // Saímos da función se conectamos con éxito
    } else {
      Serial.print(" erro, rc=");
      Serial.print(client.state());
      
      // 3. Se ambos fallan, esperamos antes de volver a intentar o ciclo completo
      Serial.println(". Todos os servidores fallaron. Reintentando en 5 segundos...");
      delay(5000);
    }
  }
}

void setup() {
  // Configuración do sensor de humidade do solo
  pinMode(sensorPin, INPUT);
  
  // Configuración do porto serie
  Serial.begin(115200);
  
  // Conexión coa WiFi
  setup_wifi();
  
  // NOTA: Eliminamos client.setServer() de aquí porque agora o xestiona reconnect()
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
