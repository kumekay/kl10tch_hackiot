#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "kuhh";
const char* password = "bubblebubble";

char* topic_temp    = "/hackiot/kettle/temperature";
char* topic_ready   = "/hackiot/kettle/ready";
char* topic_heat    = "/hackiot/kettle/heat";
char* topic_heating = "/hackiot/kettle/heating";

char* server = "iot.eclipse.org";

const int tempSensorPin = 13;
const int heaterPin = 14;
const int HEATER_ON = 1;
const int HEATER_OFF = 0;

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

OneWire oneWire(tempSensorPin);
DallasTemperature tempSensor(&oneWire);

int g_targetTemp = 0;
float g_temp = 0;

void setup(void)
{
  digitalWrite(heaterPin, HEATER_OFF);
  
  Serial.begin(115200);
  Serial.println("");
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  if (client.connect("kettle")) {
    Serial.println("Connected to MQTT broker");
    client.subscribe(topic_heat);
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }
  
  tempSensor.begin();
}


void getTemperature() {
  Serial.print("t=");
  tempSensor.requestTemperatures();
  g_temp = tempSensor.getTempCByIndex(0);
  Serial.println(g_temp);
}

void publishTemperature() {
  String payload(g_temp);
  client.publish(topic_temp, (char*) payload.c_str());
  Serial.print("Publish t=");
  Serial.println(payload);
}

void heater(int val) {
  Serial.print("Heater ");
  Serial.println((val)?"on":"off");
  digitalWrite(heaterPin, (val)?HEATER_ON:HEATER_OFF);
  String payload(val);
  client.publish(topic_heating, (char*) payload.c_str());
}

void doTempCheck() {
  if (g_targetTemp == 0)
   return;
  if (g_temp >= g_targetTemp) {
    Serial.print("Reached target=");
    Serial.print(g_targetTemp);
    Serial.print(", t=");
    Serial.println(g_temp);
    heater(0);
    g_targetTemp = 0;
    String payload(g_temp);
    client.publish(topic_ready, (char*)payload.c_str());
  }
}

void loop(void) { 
  if (!client.connected()) {
    heater(0);
    abort();
  }
    
  client.loop();
  getTemperature();
  static int count = 0;
  if (++count % 5 == 0)
    publishTemperature();
  doTempCheck();
}

void callback(char* topic, byte* payload, unsigned int length) {
  String topicStr(topic);
  payload[length] = 0;
  String payloadStr((const char*) payload);
  
  if (topicStr == topic_heat) {
    g_targetTemp = payloadStr.toInt();
    Serial.print("Got heat command, target t=");
    Serial.println(g_targetTemp);
    if (g_targetTemp > g_temp) {
      heater(1);
    }
  }
}

