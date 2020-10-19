#include <SPI.h>
#include <WiFiNINA.h>


#include "wifi_provisioning.h"

void setup() {
  Serial.begin(9600);
  while (!Serial) {
  }
  
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  
  WifiProvisioning wp;

  wp.begin(true);
}

void loop() {  
  delay(10000);
  printCurrentNet();
}


void printCurrentNet() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);
  
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}
