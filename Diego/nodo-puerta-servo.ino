#include <Arduino.h>
#include <Servo.h>

#include <WiFi.h>
#include <PubSubClient.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define SERVO_PIN 23 // D4
#define SOIL_PIN 34 // D34
#define COLLISION_PIN 34 // D

#define SENSOR_MAX 1000

#define NODE_NAME "sedigaca_entrada"

// Configuración do broker MQTT
#define FOG_SERVER "10.30.172.92"     // Fog device (Raspberry Pi)
#define CLOUDLET_SERVER "10.30.172.109" // Cloudlet (PC)
#define MQTT_PORT 1883

#define ROOM_NAME "entrada"

// Credenciais da rede Wifi
const char* ssid = "";
const char* password = "";

// Topics
#define TOPIC_DOOR "sdgc/entrada/puerta"
#define TOPIC_COLLISION "sdgc/entrada/sensor-puerta"
#define TOPIC_BT_BAND "band"
#define TOPIC_SOIL "sdgc/entrada/sensor_suelo"

// Bands
#define TARGET_MAC "d0:62:2c:2a:98:fd"

// Time for interruptions
#define TIMER_SOIL_SENSOR 10000000 // 10 seconds
#define TIMER_BAND 15000000 // 15 seconds
#define TIMER_DOOR_SENSOR 5000000 // 5 seconds

Servo myservo;
WiFiClient espClient;
PubSubClient client(espClient);

hw_timer_t *reading_timer_soil = NULL;
hw_timer_t *reading_timer_door = NULL;
hw_timer_t *bt_band_timer = NULL;

int scanTime = 5;
BLEScan* pBLEScan;

volatile bool readSoil = false;
volatile bool readDoor = false;
volatile bool detectBand = false;

char soilC[10];
int soil_moisture;

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

void IRAM_ATTR onReadingSoil() {

  readSoil = true;

}

void IRAM_ATTR onReadingDoor() {

  readDoor = true;

}

void IRAM_ATTR onDetection(){

  detectBand = true; 

}

void setup(){

  Serial.begin(115200);

  setup_wifi();
  delay(1000);

  myservo.attach(SERVO_PIN);
  pinMode(SOIL_PIN, INPUT);
  pinMode(COLLISION_PIN, INPUT);
  delay(1000);

  setup_bluetooth();
  delay(1000);

  setup_timers();
  delay(1000);

}

void setup_bluetooth() {

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true); 
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  

}

void setup_timers() {

  reading_timer_soil = timerBegin(0, 80, true);
  reading_timer_door = timerBegin(1, 80, true);
  bt_band_timer = timerBegin(2, 80, true);

  timerAttachInterrupt(reading_timer_soil, &onReadingSoil, false);
  timerAttachInterrupt(reading_timer_door, &onReadingDoor, false);
  timerAttachInterrupt(bt_band_timer, &onDetection, false);

  timerAlarmWrite(reading_timer_soil, TIMER_SOIL_SENSOR, true);
  timerAlarmWrite(reading_timer_door, TIMER_DOOR_SENSOR, true);
  timerAlarmWrite(bt_band_timer, TIMER_BAND, true);

  timerAlarmEnable(reading_timer_soil);
  timerAlarmEnable(reading_timer_door);
  timerAlarmEnable(bt_band_timer);

}

void onReadSoilValue(){

  soil_moisture = constrain( map(analogRead(SOIL_PIN), 0, SENSOR_MAX, 0, 100), 0, 100 );
  client.publish(TOPIC_SOIL, dtostrf(soil_moisture, 2, 2, soilC));

}

void onReadDoorValue(){

  int val = digitalRead(COLLISION_PIN);
  String msg = (val ? "open" : "close"); 
  client.publish(TOPIC_SOIL, msg.c_str());

}

void onDetectBand(){

  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan->clearResults();

}

void loop(){
  
  if (!client.connected())
    reconnect();
  
  client.loop();

  if (readSoil){
    readSoil = false;
    onReadSoilValue();
  }

  if (detectBand){
    detectBand = false;
    onDetectBand();
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

void reconnect() {

    Serial.print("Trying to connect to FOG (Raspberry Pi at ");
    Serial.print(FOG_SERVER);
    Serial.print(")...");

    client.setServer(FOG_SERVER, 1883);
    client.setCallback(callback);

    if (client.connect(NODE_NAME)) {

        Serial.println(" CONNECTED to Fog!");
        client.subscribe(TOPIC_DOOR);
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
        client.subscribe(TOPIC_DOOR);
        return;

    } else {

        Serial.print(" failed, rc=");
        Serial.print(client.state());

        Serial.println(". All servers failed. Retrying in 5 seconds...");
        delay(5000);
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
