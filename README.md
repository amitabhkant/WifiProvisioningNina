# Wifi Nina Provisioning

This library provides a simple provisioning workflow for your Wifi-capable Arduino device. The workflow is as follows:
1. The Arduino device is turned into an web server and an access point.
2. You connect to the Arduino Access Point with your computer or mobile phone.
3. You open a browser to http://192.168.4.1.
4. You select the Wifi network you want the Arduino to connect to.
5. You provide the Wifi password.
6. The Arduino device attempts to connect to the selected Wifi network.

The last step can be skipped if you want to use other libraries to manage the connection (ex: [Blynk](https://www.blynk.io)).

The default SSID and password for the Arduino access point are:
- SSID: arduino_ap
- Password: 12345678

You can change both when instantiating the `WifiNinaProvisioning` object. The password must be at least 8 characters.

Starting the provisioning workflow only requires calling the `WifiNinaProvisioning::begin()` function.

```
#include "wifi_nina_provisioning.h"

WifiNinaProvisioning wp("my_ssid", "my_password");
wp.begin();
```

See the _basic_ example sketch for a complete demo of the library.
