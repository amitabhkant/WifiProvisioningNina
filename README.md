# Wifi Nina Provisioning

This library provides a simple provisioning workflow for your Wifi Nina compatible Arduino device.

## Standard Provisioning Workflow

The workflow is as follows:

1. The Arduino device is turned into an access point running a web server.
2. You connect to the access point with your computer or mobile phone.
3. You open a browser to http://192.168.4.1.
4. You provide the credentials:
  - Wifi network you want the Arduino to connect to.
  - Password for that network.
  - Auth/API key if the device uses an IoT cloud service (ex : [Blynk](https://www.blynk.io))
5. The Arduino device attempts to connect to the selected Wifi network.

The workflow is triggered as follows:

```cpp
#include "wifi_nina_provisioning.h"

WifiNinaProvisioning wp();
wp.begin(true);
```

The default SSID and password of the Arduino access point are:
- SSID: arduino_ap
- Password: 12345678

## Customizing The Workflow

### Access Point credentials

You can override the default SSID and password of the access point by supplying them to the constructor:

```cpp
#include "wifi_nina_provisioning.h"

WifiNinaProvisioning wp("my_ssid", "my_password");
wp.begin(true);
```

The password must be at least 8 characters long.

### Avoid Connecting To Selected Wifi Network

If you do not want the workflow to connect to the selected network, use `wp.begin(false)` instead. Use the `get_ssid()`, `get_pass()` and `get_auth_key()`accessors to access credentials provided by the user.

This is particularly useful if you want to handle the connection yourself or if you want to delegate it to an IoT library.

## Persistent Credential Storage

The credentials provided by the user can be saved to WifiNina's permanent storage to avoid provisioning at each reboot or reset. After the initial provisioning use `store_credentials()` so save credential to the permanent storage and `retrieve_credentials()` to retrieve them from storage. Then, use the `get_ssid()`, `get_pass()` and `get_auth_key()`accessors to access the retrieved credentials.

## Examples

See the _basic_ example sketch for a complete demo of the library.
