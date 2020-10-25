#include <SPI.h>
#define BLYNK_PRINT Serial
#include <WiFiNINA.h>
#include <BlynkSimpleWiFiNINA.h>
#include "wifi_nina_provisioning.h"

#define BLYNK_API_KEY "L0JKKFe2p8k-a9hasRjxOBfBBQb_bw1j"

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

  WifiNinaProvisioning wp;
  wp.begin(false);
  //wp.retrieve_credentials();

  Serial.print("SSID: ");
  Serial.println("[" + wp.get_ssid() + "]");

  Serial.print("PASS: ");
  Serial.println("[" + wp.get_pass() + "]");

  Serial.print("AUTH KEY: ");
  Serial.println("[" + wp.get_auth_key() + "]");

  Blynk.begin(wp.get_auth_key().c_str(), wp.get_ssid().c_str(), wp.get_pass().c_str());
}

void loop() {
  Blynk.run();
}
