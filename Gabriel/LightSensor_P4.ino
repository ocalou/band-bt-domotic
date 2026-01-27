#include <PubSubClient.h>
#include <WiFi.h>

// WiFi credentials
const char* ssid = "<AP>";
const char* password = "<AP_PASS>";

const int port = 1883;

// MQTT servers
const char* fogServer = "10.30.172.92";      // Fog device (Raspberry Pi)
const char* cloudletServer = "10.30.172.109"; // Cloudlet (PC)
const char* mqtt_topic = "sdgc/salon/sensor-luminosidad";

WiFiClient espClient;
PubSubClient client(espClient);

// Hardware configuration
const int sensorPin = 3;     // Sensor pin
const int ledPin = 14;       // LED pin
const int threshold = 200;   // Light threshold

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    Serial.println("...................................");
    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected to WiFi network!");
}

// MODIFIED LOGIC: FOG -> CLOUDLET FAILOVER
void reconnect() {
    // Loop until a connection is established

    // 1. PRIMARY ATTEMPT: Connect to Fog device (Raspberry Pi)
    Serial.print("Trying to connect to FOG (Raspberry Pi at ");
    Serial.print(fogServer);
    Serial.print(")...");

    // Set Fog server
    client.setServer(fogServer, 1883);

    if (client.connect("lightSensorID")) { // Use a unique client ID
        Serial.println(" CONNECTED to Fog!");
        client.subscribe(mqtt_topic);
        return; // Exit function on successful connection
    } else {
        Serial.print(" failed, rc=");
        Serial.print(client.state());
        Serial.println(". Trying Cloudlet...");
    }

    // 2. SECONDARY ATTEMPT (FAILOVER): Connect to Cloudlet
    Serial.print("Trying to connect to CLOUDLET (PC at ");
    Serial.print(cloudletServer);
    Serial.print(")...");

    // Change server to Cloudlet
    client.setServer(cloudletServer, 1883);

    if (client.connect("lightSensorID")) {
        Serial.println(" CONNECTED to Cloudlet!");
        client.subscribe(mqtt_topic);
        return; // Exit function on successful connection
    } else {
        Serial.print(" failed, rc=");
        Serial.print(client.state());

        // 3. If both fail, wait before retrying
        Serial.println(". All servers failed. Retrying in 5 seconds...");
        delay(5000);
    }
}

void loop() {

    pinMode(ledPin, OUTPUT);  // Set LED pin as output

    // Check MQTT connection
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    // Read sensor value
    int sensorValue = analogRead(sensorPin);

    // Convert sensor value to string
    char str[16];
    sprintf(str, "%u", sensorValue);

    // Publish sensor value (same topic used in Node-RED)
    client.publish(mqtt_topic, str);
    Serial.println(str);

    // LED control logic based on threshold
    if (sensorValue > threshold) {
        digitalWrite(ledPin, LOW);   // Turn LED OFF
    } else {
        digitalWrite(ledPin, HIGH);  // Turn LED ON
    }

    delay(5000);
}
