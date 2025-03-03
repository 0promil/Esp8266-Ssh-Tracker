Overview
This project uses an ESP8266 microcontroller to create a Wi-Fi access point (AP) where users can connect and enter their Wi-Fi credentials (SSID and password). Once the credentials are provided, the ESP8266 connects to the given network and restarts. This system is useful for situations where you need to set up a device without physical access, such as an IoT device that requires Wi-Fi credentials.

Requirements
ESP8266 development board (e.g., NodeMCU, Wemos D1 Mini)
Arduino IDE (or another compatible IDE for ESP8266)
Libraries:
ESP8266WiFi
ESPAsyncWebServer
ESP8266HTTPClient
WiFiUdp
PubSubClient
DNSServer
ESPAsyncTCP
Installation Steps
Install Arduino IDE
If you haven't already, download and install the Arduino IDE.

Install ESP8266 Board Package
Go to the Arduino IDE, click File -> Preferences. In the "Additional Boards Manager URLs" section, add:

bash
Kopyala
http://arduino.esp8266.com/stable/package_esp8266com_index.json
Then go to Tools -> Board -> Boards Manager, search for ESP8266 and install the package.

Install Required Libraries
In the Arduino IDE, go to Sketch -> Include Library -> Manage Libraries and install the following libraries:

ESP8266WiFi
ESPAsyncWebServer
ESP8266HTTPClient
WiFiUdp
PubSubClient
DNSServer
ESPAsyncTCP
Upload the Code to the ESP8266
Select the correct board and port from Tools -> Board and Tools -> Port, then upload the provided code to your ESP8266.

Connect to the ESP8266 AP
Once the ESP8266 is powered on, it will start an access point called SSH_Tracker_Setup. Connect to this Wi-Fi network with any device.

Enter Wi-Fi Credentials
After connecting to the ESP8266's Wi-Fi, navigate to the following IP address in your browser:

Kopyala
192.168.4.1
You will be presented with a page where you can enter your home Wi-Fi SSID and password.

Reconnect
After entering the credentials, the ESP8266 will attempt to connect to the entered network. If successful, it will restart. If the connection fails, you can try again.

Logs
You can view the logs by navigating to:

arduino
Kopyala
http://192.168.4.1/logs
This will show you any debug information related to the Wi-Fi connection process.
