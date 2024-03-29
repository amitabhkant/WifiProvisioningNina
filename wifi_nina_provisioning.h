#ifndef _WIFI_NINA_PROVISIONING_
#define _WIFI_NINA_PROVISIONING_

#include <WiFiNINA.h>
#include <Regexp.h>

#define DEFAULT_SSID              "arduino_ap"
#define DEFAULT_PASS              "12345678"
#define DEFAULT_WEB_SERVER_PORT   80
#define MAX_NETWORKS              20
#define DELAY_AP_CREATION         10000
#define DELAY_CONNECTION_ATTEMPTS 10000
#define SSID_FILE                 "/fs/ssid"
#define PASS_FILE                 "/fs/pass"
#define AUTH_KEY_FILE             "/fs/auth_key"

class WifiNinaProvisioning {
  private:
    String      ap_ssid;
    String      ap_pass;
    String      ssid;
    String      pass;
    String      auth_key;
    int         status;
    WiFiServer  *server;
    String      networks[MAX_NETWORKS];

  public:
    WifiNinaProvisioning();
    WifiNinaProvisioning(String _ap_ssid, String _ap_pass);
    void        begin(bool connect);
    String      get_ssid();
    String      get_pass();
    String      get_auth_key();
    void        store_credentials();
    void        retrieve_credentials();
    void        erase_credentials();

  private:
    void        init();
    void        scan_networks();
    void        create_ap();
    void        start_web_server();
    void        connect_to_network();

    void        print_html_networks(WiFiClient client);
    void        print_html_connecting(WiFiClient client);
    void        get_network_credetials(String request);

    void        print_ap_status();
    void        print_network_status();
};

#endif
