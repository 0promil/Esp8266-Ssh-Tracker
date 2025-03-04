#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <DNSServer.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#include <LittleFS.h>

const char* apSSID = "SSH_Tracker_Setup";
const char* apPassword = "12345678";
DNSServer dnsServer;
AsyncWebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
String userSSID = "";
String userPassword = "";
String deviceIP = "";
String sshServerIP = "";
String sshServerPort = "";
String sshPassword = "";

void startAP() {
    WiFi.softAP(apSSID, apPassword);
    dnsServer.start(53, "captive.portal", WiFi.softAPIP());
    Serial.println("Access Point Started");
}

void logSSHConnection(String log) {
    File logFile = LittleFS.open("/logs.txt", "a");
    if (logFile) {
        logFile.println(log);
        logFile.close();
    }
    Serial.println(log);
}

String getLogs() {
    File logFile = LittleFS.open("/logs.txt", "r");
    String logs = "";
    if (logFile) {
        while (logFile.available()) {
            logs += logFile.readStringUntil('\n') + "\n";
        }
        logFile.close();
    }
    return logs;
}

String getHtml() {
    return "<!DOCTYPE html>"
           "<html lang='en'>"
           "<head>"
           "<meta charset='UTF-8'>"
           "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
           "<title>SSH Tracker</title>"
           "<style>"
           "body { font-family: Arial, sans-serif; text-align: center; background-color: #121212; color: white; padding: 20px; }"
           "button { padding: 10px; margin: 10px; border: none; background-color: #008CBA; color: white; font-size: 18px; cursor: pointer; border-radius: 5px; }"
           "button:hover { background-color: #005f73; }"
           "input { padding: 10px; margin: 5px; border-radius: 5px; border: none; }"
           "#content { margin-top: 20px; padding: 20px; border-radius: 10px; background-color: #222; display: inline-block; }"
           "#github { position: absolute; top: 10px; left: 10px; background: #333; padding: 8px; border-radius: 5px; text-decoration: none; color: white; }"
           "</style>"
           "</head>"
           "<body>"
           "<a id='github' href='https://github.com/0promil' target='_blank'>GitHub</a>"
           "<h1>SSH Tracker Setup</h1>"
           "<div id='content'>"
           "<button onclick='showSettings()'>Ayarlar</button>"
           "<button onclick='showLogs()'>Loglar</button>"
           "<div id='settings' style='display:none;'>"
           "<form action='/connect' method='POST'>"
           "SSID: <input type='text' name='ssid'><br>"
           "Password: <input type='password' name='password'><br>"
           "Cihaz IP: <input type='text' name='deviceip'><br>"
           "SSH Sunucu IP: <input type='text' name='sship'><br>"
           "SSH Port: <input type='text' name='sshport'><br>"
           "SSH Şifre (Opsiyonel): <input type='password' name='sshpassword'><br>"
           "<input type='submit' value='Bağlan'>"
           "</form>"
           "</div>"
           "<div id='logs' style='display:none;'>"
           "<pre id='logContent'></pre>"
           "</div>"
           "</div>"
           "<script>"
           "function showSettings() { document.getElementById('settings').style.display = 'block'; document.getElementById('logs').style.display = 'none'; }"
           "function showLogs() { document.getElementById('settings').style.display = 'none'; document.getElementById('logs').style.display = 'block'; fetch('/logs').then(response => response.text()).then(data => document.getElementById('logContent').innerText = data); }"
           "</script>"
           "</body>"
           "</html>";
}

void setup() {
    Serial.begin(115200);
    if (!LittleFS.begin()) {
        Serial.println("LittleFS Mount Failed");
        return;
    }
    startAP();

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", getHtml());
    });
    
    server.on("/connect", HTTP_POST, [](AsyncWebServerRequest *request){
        if (request->hasParam("ssid", true) && request->hasParam("password", true) && request->hasParam("deviceip", true) && request->hasParam("sship", true) && request->hasParam("sshport", true)) {
            userSSID = request->getParam("ssid", true)->value();
            userPassword = request->getParam("password", true)->value();
            deviceIP = request->getParam("deviceip", true)->value();
            sshServerIP = request->getParam("sship", true)->value();
            sshServerPort = request->getParam("sshport", true)->value();
            sshPassword = request->hasParam("sshpassword", true) ? request->getParam("sshpassword", true)->value() : "";
            
            WiFi.softAPdisconnect(true);
            WiFi.begin(userSSID.c_str(), userPassword.c_str());
            int attempts = 0;
            while (WiFi.status() != WL_CONNECTED && attempts < 20) {
                delay(500);
                Serial.print(".");
                attempts++;
            }
            if (WiFi.status() == WL_CONNECTED) {
                request->send(200, "text/html", "Connected! Restarting...");
                delay(2000);
                ESP.restart();
            } else {
                startAP();
                request->send(200, "text/html", "Failed! Try again.");
            }
        }
    });

    server.on("/logs", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", getLogs());
    });
    
    server.begin();
}

void loop() {
    dnsServer.processNextRequest();
    logSSHConnection("Example SSH connection detected.");
}
