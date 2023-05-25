#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <MAX31855.h> // Include MAX31855 Sensor library

const uint32_t SERIAL_SPEED     = 115200; ///< Set the baud rate for Serial I/O
const uint8_t  SENSOR1_CS_PIN   = 9;
const uint8_t  SENSOR2_CS_PIN   = 8;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(172, 17, 170, 194);
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

MAX31855_Class sensor1;
MAX31855_Class sensor2;

void setup() {
  Serial.begin(SERIAL_SPEED);
  while (!sensor1.begin(SENSOR1_CS_PIN)) {
    Serial.println(F("Unable to start sensor1. Waiting 3 seconds."));
  }
  while (!sensor2.begin(SENSOR2_CS_PIN)) {
    Serial.println(F("Unable to start sensor2. Waiting 3 seconds."));
  }
  Ethernet.begin(mac);
  mqttClient.setServer(server, 1883);
}


void loop() {
  int32_t probe1 = sensor1.readProbe();
  int32_t probe2 = sensor2.readProbe();
  StaticJsonDocument<32> doc;
  char output[55];
  doc["a"] = ((float)probe1 / 1000);
  doc["b"] = ((float)probe2 / 1000);
  Serial.println(output);
  if (mqttClient.connect("arduinoClient")) {
    Serial.println("Connected to MQTT server");
    serializeJson(doc, output);
    mqttClient.publish("topic", output);
    Serial.println("Sent");
    mqttClient.disconnect();
  }
  delay(500);
}