#include <Arduino.h>
#include <Servo.h>

#include <WiFi.h>
#include <PubSubClient.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <IRremote.h>

#define ROOM_NAME "salon"

#define RELE_PIN 32 // D32
#define IR_PIN 19 // D33
#define LIGHT_PIN 34  // D33

#define NODE_NAME "sedigaca_salon"

// Configuración do broker MQTT
#define FOG_SERVER "10.30.172.92"     // Fog device (Raspberry Pi)
#define CLOUDLET_SERVER "10.30.172.109" // Cloudlet (PC)
#define MQTT_PORT 1883

// Credenciais da rede Wifi
const char* ssid = "";
const char* password = "";

// Topics
#define TOPIC_LIGHT_SENSOR "sdgc/salon/sensor-luminosidad"
#define TOPIC_IR "sdgc/salon/tv"
#define TOPIC_BT_BAND "band"
#define TOPIC_RELE "sdgc/salon/iluminacion"

// Bands
#define TARGET_MAC "d0:62:2c:2a:98:fd"

// Time for interruptions
#define TIMER_LIGHT_SENSOR 10000000 // 10 seconds
#define TIMER_BAND 15000000 // 15 seconds

Servo myservo;
WiFiClient espClient;
PubSubClient client(espClient);

hw_timer_t *reading_timer_light = NULL;
hw_timer_t *bt_band_timer = NULL;

int scanTime = 5;
BLEScan* pBLEScan;

volatile bool readLight = false;
volatile bool detectBand = false;

IRsend irsend;

char lightC[10];
int light;

void setup_wifi();
void setup_timers();
void setup_bluetooth();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {

    void onResult(BLEAdvertisedDevice advertisedDevice) {

      String currentAddress = advertisedDevice.getAddress().toString().c_str();
      currentAddress.toLowerCase();

      if (currentAddress == TARGET_MAC) {

        String cleanAddress = currentAddress;
        cleanAddress.replace(":", "");

        String fullTopic = String(TOPIC_BT_BAND) + "/" + cleanAddress + "/" + ROOM_NAME;

        char rssiPayload[10];
        dtostrf(advertisedDevice.getRSSI(), 2, 2, rssiPayload);

        client.publish(fullTopic.c_str(), rssiPayload);
            
      }
    }

};

void IRAM_ATTR onReadingLight() {

  readLight = true;

}

void IRAM_ATTR onDetection(){

  detectBand = true; 

}

void setup() {

  Serial.begin(115200);
  delay(1000);

  setup_wifi();
  delay(1000);

  pinMode(LIGHT_PIN, INPUT);
  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, LOW);
  
  IrSender.begin(IR_PIN); 
  delay(1000);

  setup_bluetooth();
  delay(1000);

  setup_timers();
  delay(1000);
  
}

void onreadLightValue(){

  int sensorValue = analogRead(LIGHT_PIN);

  if (sensorValue)
    client.publish(TOPIC_LIGHT_SENSOR, dtostrf(sensorValue, 2, 2, lightC));

}

void onDetectBand(){

  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan->clearResults();

}

void loop() {

  if (!client.connected())
    reconnect();

  client.loop();

  if (readLight){
    readLight = false;
    onreadLightValue();
  }

  if (detectBand){
    detectBand = false;
    pBLEScan->start(1, false); 
  }

}

void setup_wifi() {

  delay(10);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi error");
    delay(1000);
  }

}

void setup_bluetooth() {

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); 
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  

}

void setup_timers(){

  reading_timer_light = timerBegin(0, 80, true);
  bt_band_timer = timerBegin(1, 80, true);

  timerAttachInterrupt(reading_timer_light, &onReadingLight, false);
  timerAttachInterrupt(bt_band_timer, &onDetection, false);

  timerAlarmWrite(reading_timer_light, TIMER_LIGHT_SENSOR, true);
  timerAlarmWrite(bt_band_timer, TIMER_BAND, true);

  timerAlarmEnable(reading_timer_light);
  timerAlarmEnable(bt_band_timer);

}

void reconnect() {

    Serial.print("Trying to connect to FOG (Raspberry Pi at ");
    Serial.print(FOG_SERVER);
    Serial.print(")...");

    client.setServer(FOG_SERVER, 1883);
    client.setCallback(callback);

    if (client.connect(NODE_NAME)) {

        Serial.println(" CONNECTED to Fog!");
        client.subscribe(TOPIC_RELE);
        client.subscribe(TOPIC_IR);
        return;

    } else {
        Serial.print(" failed, rc=");
        Serial.print(client.state());
        Serial.println(". Trying Cloudlet...");
    }

    Serial.print("Trying to connect to CLOUDLET (PC at ");
    Serial.print(CLOUDLET_SERVER);
    Serial.print(")...");

    // Change server to Cloudlet
    client.setServer(CLOUDLET_SERVER, MQTT_PORT);

    if (client.connect("lightSensorID")) {

        Serial.println(" CONNECTED to Cloudlet!");
        client.subscribe(TOPIC_RELE);
        client.subscribe(TOPIC_IR);
        return;

    } else {

        Serial.print(" failed, rc=");
        Serial.print(client.state());

        Serial.println(". All servers failed. Retrying in 5 seconds...");
        delay(5000);
    }
}

void callback(char* topic, byte* payload, unsigned int length) {

  String msg = "";
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  if (String(topic) == TOPIC_RELE) {
    digitalWrite(RELE_PIN, (msg == "1") ? HIGH : LOW);
  }

  if (String(topic) == TOPIC_IR) {
    if (msg == "1")
      IrSender.sendRC5(0x01, 12);
    else
      IrSender.sendRC5(0x00, 12);
    Serial.println("IR enviado");
  }

}
