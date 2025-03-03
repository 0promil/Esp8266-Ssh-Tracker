#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>

const char* ssid = "Your_WiFi_SSID";
const char* password = "Your_WiFi_Password";
const char* serverUrl = "http://your-server.com/log";
const char* telegramBotToken = "YOUR_TELEGRAM_BOT_TOKEN";
const char* telegramChatID = "YOUR_CHAT_ID";
const char* mqttServer = "YOUR_MQTT_BROKER";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer server(80);
WiFiUDP udp;

String logs = "";

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    
    client.setServer(mqttServer, mqttPort);
    connectMQTT();

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", "<html><body><h2>SSH Tracker Logs</h2><pre>" + logs + "</pre></body></html>");
    });
    server.begin();
}

void connectMQTT() {
    while (!client.connected()) {
        if (client.connect("ESP8266Client")) {
            Serial.println("Connected to MQTT");
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" retrying...");
            delay(5000);
        }
    }
}

void scanNetwork() {
    int n = WiFi.scanNetworks();
    Serial.println("Scanning...");
    for (int i = 0; i < n; ++i) {
        String ssid = WiFi.SSID(i);
        int rssi = WiFi.RSSI(i);
        Serial.printf("Network: %s | Signal: %d dBm\n", ssid.c_str(), rssi);
    }
}

void sendLog(String data) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(espClient, serverUrl);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST(data);
        Serial.printf("Log Sent | Response: %d\n", httpResponseCode);
        http.end();
    }
}

void sendTelegramMessage(String message) {
    String url = "https://api.telegram.org/bot" + String(telegramBotToken) + "/sendMessage?chat_id=" + telegramChatID + "&text=" + message;
    HTTPClient http;
    http.begin(espClient, url);
    int httpResponseCode = http.GET();
    Serial.printf("Telegram Response: %d\n", httpResponseCode);
    http.end();
}

void mqttPublish(String topic, String message) {
    if (!client.connected()) {
        connectMQTT();
    }
    client.publish(topic.c_str(), message.c_str());
}

void loop() {
    scanNetwork();
    logs += "SSH activity detected\n";
    sendLog("{\"log\": \"SSH activity detected\"}");
    sendTelegramMessage("SSH activity detected on your network!");
    mqttPublish("ssh_tracker/logs", "SSH activity detected");
    delay(60000);
}
