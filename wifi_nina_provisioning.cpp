#include "wifi_nina_provisioning.h"

WifiNinaProvisioning::WifiNinaProvisioning() {
  ap_ssid = DEFAULT_SSID;
  ap_pass = DEFAULT_PASS;

  init();
}

WifiNinaProvisioning::WifiNinaProvisioning(String _ap_ssid, String _ap_pass) {
  ap_ssid = _ap_ssid;
  ap_pass = _ap_pass;

  init();
}

void WifiNinaProvisioning::init() {
  status = WL_IDLE_STATUS;
  server = new WiFiServer(DEFAULT_WEB_SERVER_PORT);

  for (int i = 0; i < MAX_NETWORKS; i++) {
    networks[i] = "";
  }
}

String WifiNinaProvisioning::get_ssid() {
  return ssid;
}

String WifiNinaProvisioning::get_pass() {
  return pass;
}

String WifiNinaProvisioning::get_auth_key() {
  return auth_key;
}

void WifiNinaProvisioning::begin(bool connect) {
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

void WifiNinaProvisioning::scan_networks() {
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

void WifiNinaProvisioning::create_ap() {
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

void WifiNinaProvisioning::start_web_server() {
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

void WifiNinaProvisioning::print_html_networks(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  // the content of the HTTP response follows the header:
  client.print("<h2>Specify Network & Credentials</h2>");
  client.print("<form action=\"/connect\" method=\"get\">");
  client.print("<label for=\"network\">Network:</label>");
  client.print("<select name=\"network\" id=\"network\">");
  for (int i = 0; i < MAX_NETWORKS; i++) {
    if (networks[i] == "") {
      break;
    }
    client.print("<option value=\"" + networks[i] + "\">" + networks[i] + "</option>");
  }
  client.print("</select><br><br>");
  
  client.print("<label for=\"password\">Password:</label>");
  client.print("<input type=\"text\" id=\"password\" name=\"password\"><br><br>");
  
  client.print("<label for=\"auth_key\">Auth/API Key:</label>");
  client.print("<input type=\"text\" id=\"auth_key\" name=\"auth_key\"><br><br>");
  
  client.print("<input type=\"submit\" value=\"Connect\">");
  client.print("</form>");

  // The HTTP response ends with another blank line:
  client.println();
}

void WifiNinaProvisioning::print_html_connecting(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.print("We will now connect to " + ssid + ".");
  client.println();
}

void WifiNinaProvisioning::get_network_credetials(String request) {
  // GET /connect?network=pogui&password=pibedeoro&auth_key=ewqfvdsa HTTP/1.1
  request = request.substring(0, request.indexOf("\n")); // Get first line of request
  request = request.substring(13, request.length() - 9); // Trim
  int         l = request.length() + 1;
  char        s[l];
  
  request.toCharArray(s, l);
  MatchState ms(s);
  
  ms.Match("^network=(.-)&password=(.-)&auth_key=(.-)$");

  char cap[128];
  ms.GetCapture(cap, 0);
  ssid = cap;
  
  ms.GetCapture(cap, 1);
  pass = cap;
  
  ms.GetCapture(cap, 2);
  auth_key = cap;
  
  Serial.println("  - SSID: " + ssid);
  Serial.println("  - PASS: " + pass);
  Serial.println("  - AUTH KEY: " + auth_key);
}

void WifiNinaProvisioning::connect_to_network() {
  Serial.println("Connecting to selected network...");

  while (status != WL_CONNECTED) {
    Serial.print("  Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid.c_str(), pass.c_str());
    delay(DELAY_CONNECTION_ATTEMPTS);
  }
  print_network_status();
}

void WifiNinaProvisioning::store_credentials() {
  WiFiStorageFile ssid_file = WiFiStorage.open(SSID_FILE);
  WiFiStorageFile pass_file = WiFiStorage.open(PASS_FILE);
  WiFiStorageFile auth_key_file = WiFiStorage.open(AUTH_KEY_FILE);
    
  if (ssid_file) {
    ssid_file.erase();
  }

  if (pass_file) {
    pass_file.erase();
  }

  if (auth_key_file) {
    auth_key_file.erase();
  }

  ssid_file.write(ssid.c_str(), ssid.length());
  pass_file.write(pass.c_str(), pass.length());
  auth_key_file.write(auth_key.c_str(), auth_key.length());
}

void WifiNinaProvisioning::retrieve_credentials() {
  WiFiStorageFile ssid_file = WiFiStorage.open(SSID_FILE);
  WiFiStorageFile pass_file = WiFiStorage.open(PASS_FILE);
  WiFiStorageFile auth_key_file = WiFiStorage.open(AUTH_KEY_FILE);
  char buf[128];
  int l;
  
  if (ssid_file) {
    ssid_file.seek(0);
    while (ssid_file.available()) {
      l = ssid_file.read(buf, 128);
      delay(3000);
    }
    ssid = buf;
    ssid = ssid.substring(0, l);
  }

  if (pass_file) {
    pass_file.seek(0);
    while (pass_file.available()) {
      l = pass_file.read(buf, 128);
      delay(3000);
    }
    pass = buf;
    pass= pass.substring(0, l);
  }

  if (auth_key_file) {
    auth_key_file.seek(0);
    while (auth_key_file.available()) {
      l = auth_key_file.read(buf, 128);
      delay(3000);
    }
    auth_key = buf;
    auth_key= auth_key.substring(0, l);
  }
}

void WifiNinaProvisioning::print_ap_status() {
  Serial.print("  SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("  Open a browser to http://");
  Serial.print(ip);
  Serial.println(" and choose the network you want to connect to.");
}

void WifiNinaProvisioning::print_network_status() {
  Serial.print("  You're connected to ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("  IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("  Signal strength (RSSI): ");
  Serial.println(rssi);
}
