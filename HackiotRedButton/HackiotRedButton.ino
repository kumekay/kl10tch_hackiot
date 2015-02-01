#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "DHT.h"

//const char* ssid = "gelyabka";
//const char* password = "g2222222";
const char* ssid = "kuhh";
const char* password = "bubblebubble";

char* server = "iot.eclipse.org";

int buttonState = LOW;
int previousState = LOW;
bool pressed = false; 
long time = 0; 
long debounce = 200; 
#define buttonPin 2  
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}
void callback(char* topic, byte*payload, unsigned int length) {
  char tmp[10];
  length = min(length, sizeof(tmp) - 1);
  memcpy(tmp, payload, length);
  tmp[length] = 0;
  String payload_str(tmp);
  String topic_str(topic);
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(payload_str);
}

void setup() {
  Serial.begin(115200);  
  delay(10);
    
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Generate client name based on MAC address and last 8 bits of microsecond counter
  String clientName;
  clientName += "esp8266-";
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName += "-";
  clientName += String(micros() & 0xff, 16);

  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);
  
  if (client.connect((char*) clientName.c_str())) {
    Serial.println("Connected to MQTT broker"); 
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }
  
   pinMode(buttonPin, INPUT);
}

void loop() {
static int counter = 0;
buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH && previousState == LOW && millis() - time > debounce) {
      pressed = true;
  }    
  else {
      pressed = false;  
  }
  time = millis();  
  previousState = buttonState;

 if (pressed == true) {  
  Serial.println("Button pressed!");
  
  if (client.publish("/hackiot/button0", "pressed")) {
    Serial.println("MQTT sent: Button pressed!");
  }
  
}
else {
    
}
  ++counter;
  client.loop();
}


