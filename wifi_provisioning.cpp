#include "wifi_provisioning.h"

WifiProvisioning::WifiProvisioning() {
  ap_ssid = "arduino_ap";
  ap_pass = "12345678";

  init();
}

WifiProvisioning::WifiProvisioning(String _ap_ssid, String _ap_pass) {
  ap_ssid = _ap_ssid;
  ap_pass = _ap_pass;

  init();
}

void WifiProvisioning::init() {
  status = WL_IDLE_STATUS;
  server = new WiFiServer(80);

  for (int i = 0; i < MAX_NETWORKS; i++) {
    networks[i] = "";
  }
}

String WifiProvisioning::get_ssid() {
  return ssid;
}

String WifiProvisioning::get_pass() {
  return pass;
}

void WifiProvisioning::begin(bool connect) {
  scan_networks();
  Serial.println("");
  create_ap();
  Serial.println("");
  start_web_server();
  if (connect) {
    Serial.println(""); 
    connect_to_network();
  }
}

void WifiProvisioning::scan_networks() {
  Serial.println("Scanning wifi networks...");

  int n_ssid = WiFi.scanNetworks();

  if (n_ssid == -1) {
    Serial.println("  Couldn't get a wifi connection");
  } else {
    Serial.println("  Network(s) found:");
    for (int i = 0; i < n_ssid && i < MAX_NETWORKS; i++) {
      networks[i] = WiFi.SSID(i);
      Serial.println("  - " + networks[i]);
    }
  }
}

void WifiProvisioning::create_ap() {
  Serial.println("Creating access point...");
  Serial.println("  SSID: " + ap_ssid);
  Serial.println("  PASS: " + ap_pass);

  status = WiFi.beginAP(ap_ssid.c_str(), ap_pass.c_str());

  if (status != WL_AP_LISTENING) {
    Serial.println("  Creating access point failed.");
    while (true);
  }
  delay(DELAY_AP_CREATION);

  Serial.println("  Success.");
}

void WifiProvisioning::start_web_server() {
  bool brk = false;

  Serial.println("Starting Web Server...");
  server->begin();
  Serial.println("  Success.");
  print_ap_status();

  Serial.println("\n  Waiting for clients...");
  while (true) {
    WiFiClient client = server->available();

    if (client) {
      Serial.println("  New client connected.\n");
      String currentLine = "";
      String request = "";
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          Serial.write(c);
          if (c == '\n') {
            request += currentLine + "\n";
            if (currentLine.length() == 0) { // Second empty line in a row => Process the request
              if (request.startsWith("GET /connect")) {
                get_network_credetials(request);
                print_html_connecting(client);
                brk = true;
              } else {
                Serial.println("  Display list of networks.");
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
      Serial.println("\n  Client disconnected.");
      if (brk) {
        break;
      }
    }
  }
}

void WifiProvisioning::print_html_networks(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  // the content of the HTTP response follows the header:
  client.print("<h2>Select the network you want to connect to</h2>");
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
  client.print(" <input type=\"submit\" value=\"Connect\">");
  client.print("</form>");

  // The HTTP response ends with another blank line:
  client.println();
}

void WifiProvisioning::print_html_connecting(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.print("We will now connect to " + ssid + ".");
  client.println();
}

void WifiProvisioning::get_network_credetials(String request) {
  // GET /connect?networks=pogui&password=pibedeoro HTTP/1.1
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

  Serial.println("  Received wifi credentials:");
  Serial.println("  - SSID: " + _ssid);
  Serial.println("  - PASS: " + _pass);

  ssid = _ssid;
  pass = _pass;
}

void WifiProvisioning::connect_to_network() {
  Serial.println("Connecting to selected network...");

  while (status != WL_CONNECTED) {
    Serial.print("  Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid.c_str(), pass.c_str());
    delay(DELAY_CONNECTION_ATTEMPTS);
  }
  print_network_status();
}

void WifiProvisioning::print_ap_status() {
  Serial.print("  SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("  Open a browser to http://");
  Serial.print(ip);
  Serial.println(" and choose the network you want to connect to.");
}

void WifiProvisioning::print_network_status() {
  Serial.print("  You're connected to ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("  IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("  Signal strength (RSSI): ");
  Serial.println(rssi);
}
