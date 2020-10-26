# Wifi Nina Provisioning

This library provides a simple provisioning workflow for your Wifi-capable Arduino device. The workflow is as follows:
1. The Arduino device is turned into an access point running a web server.
2. You connect to the access point with your computer or mobile phone.
3. You open a browser to http://192.168.4.1.
4. You select the Wifi network you want the Arduino to connect to.
5. You provide the Wifi password for that network.
6. You can also provide an Auth/API key if the device uses an IoT cloud service (ex : [Blynk](https://www.blynk.io))
7. (optional) The Arduino device attempts to connect to the selected Wifi network.

The default SSID and password for the Arduino access point are:
- SSID: arduino_ap
- Password: 12345678

You can change both when instantiating the `WifiNinaProvisioning` object. The password must be at least 8 characters.

Starting the provisioning workflow only requires calling the `WifiNinaProvisioning::begin()` function.

```cpp
#include "wifi_nina_provisioning.h"

WifiNinaProvisioning wp("my_ssid", "my_password");
wp.begin(true);
```

If you do not want the workflow to connect to the selected network, use `wp.begin(false)` instead. Use the `get_ssid()`, `get_pass()` and `get_auth_key()`accessors to retrieve credentials provided by the user.

The credentials can be saved to WifiNina's permanent storage to avoid provisioning at each reboot. After the initial provisioning use `store_credentials()` so save credential to the permanent storage and `retrieve_credentials()` to retrieve them from storage.

See the _basic_ example sketch for a complete demo of the library.
