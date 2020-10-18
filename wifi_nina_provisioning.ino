#include <SPI.h>
#include <WiFiNINA.h>

#define MAX_NETWORKS  20
String  networks[MAX_NETWORKS];

char ap_ssid[] = "arduino_ap";
char ap_pass[] = "12345678";

String ssid = "";
String pass = "";

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  for (int i = 0; i < MAX_NETWORKS; i++) {
    networks[i] = "";
  }
  
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

  scan_networks();
   
  Serial.print("Creating access point named: ");
  Serial.println(ap_ssid);

  status = WiFi.beginAP(ap_ssid, ap_pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    while (true);
  }
  delay(10000);

  server.begin();

  printWiFiStatus();

  connect_to_network();
  Serial.println("");
  Serial.println("READY TO CONNECT");

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid.c_str(), pass.c_str());
    // wait 10 seconds for connection:
    delay(10000);
  }
  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");

  printCurrentNet();

  printWifiData();
}

void loop() {  
//  if (status != WiFi.status()) {
//    // it has changed update the variable
//    status = WiFi.status();
//
//    if (status == WL_AP_CONNECTED) {
//      // a device has connected to the AP
//      Serial.println("Device connected to AP");
//    } else {
//      // a device has disconnected from the AP, and we are back in listening mode
//      Serial.println("Device disconnected from AP");
//    }
//  }

  // check the network connection once every 10 seconds:

  delay(10000);

  printCurrentNet();
}

void scan_networks() {
  int n_ssid = WiFi.scanNetworks();
  if (n_ssid == -1) {
    Serial.println("Couldn't get a wifi connection");
  }

  int i;
  for (i = 0; i < n_ssid && i < MAX_NETWORKS; i++) {
    networks[i] = WiFi.SSID(i);
    Serial.println(networks[i]);
  }
}

void printWiFiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void connect_to_network() {
  bool brk = false;
  while (true) {
    WiFiClient client = server.available();

    if (client) {                             
      Serial.println("new client");
      String currentLine = "";
      String request = "";
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          Serial.write(c);
          if (c == '\n') {
            request += currentLine + "\n";
            if (currentLine.length() == 0) { // Process the request
              if (request.startsWith("GET /connect")) {
                Serial.println("THIS IS CONNECT REQUEST");
                print_html_connecting(client);
                get_network_credetials(request);
                brk = true;
              } else {
                print_html_networks(client);
              }
              break;
            }
            else {
              currentLine = "";
            }
          }
          else if (c != '\r') {
            currentLine += c;
          }
        }
      }
      // close the connection:
      client.stop();
      Serial.println("client disconnected");
      if (brk) {
        break;
      }
    }
  }
}

void print_html_networks(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  
  // the content of the HTTP response follows the header:
  client.print("<form action=\"/connect\" method=\"get\">");
  client.print("<label for=\"networks\">Networks:</label>");
  client.print("<select name=\"networks\" id=\"networks\">");
  client.print("<option value=\"\"></option>");
  for (int i = 0; i < MAX_NETWORKS; i++) {
    if (networks[i] == "") {
      break;
    }
    client.print("<option value=\"" + networks[i] + "\">" + networks[i] + "</option>");
  }
  client.print("</select><br><br>");
  client.print("<label for=\"password\">Password:</label>");
  client.print("<input type=\"text\" id=\"password\" name=\"password\"><br><br>");
  client.print(" <input type=\"submit\" value=\"Submit\">");
  client.print("</form>");
  
  // The HTTP response ends with another blank line:
  client.println();
}

void print_html_connecting(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.print("We will now connect to the selected network.");
  client.println(); 
}

// GET /connect?networks=pogui&password=pibedeoro HTTP/1.1
void get_network_credetials(String request) {
  // Get first line of request and trim
  String s = request.substring(0, request.indexOf("\n"));
  s = s.substring(13, s.length() - 9);

  // Get SSID
  int start_c = s.indexOf("=");
  int end_c = s.indexOf("&");
  String _ssid = s.substring(start_c + 1, end_c);

  //Get Password
  s = s.substring(end_c + 1, s.length());
  String _pass = s.substring(s.indexOf("=") + 1, s.length());
  
  Serial.println("SSID: " + _ssid);
  Serial.println("PASS: " + _pass);

  ssid = _ssid;
  pass = _pass;
}









void printWifiData() {

  // print your board's IP address:

  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");

  Serial.println(ip);

  Serial.println(ip);

  // print your MAC address:

  byte mac[6];

  WiFi.macAddress(mac);

  Serial.print("MAC address: ");

  //printMacAddress(mac);
}

void printCurrentNet() {

  // print the SSID of the network you're attached to:

  Serial.print("SSID: ");

  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:

  byte bssid[6];

  WiFi.BSSID(bssid);

  Serial.print("BSSID: ");

  //printMacAddress(bssid);

  // print the received signal strength:

  long rssi = WiFi.RSSI();

  Serial.print("signal strength (RSSI):");

  Serial.println(rssi);

  // print the encryption type:

  byte encryption = WiFi.encryptionType();

  Serial.print("Encryption Type:");

  Serial.println(encryption, HEX);

  Serial.println();
}
