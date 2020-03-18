#include "WiFi.h"

const char* ssid     = "TR-2";
const char* password = "1234";

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
  delay(100);
}

void loop(){
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
  delay(1000);
}
