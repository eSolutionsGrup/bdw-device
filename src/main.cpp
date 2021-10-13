#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char *mqttServer = "192.168.0.130";
const int mqttPort = 1883;
const char *mqttUser = "ikjeozws";
const char *mqttPassword = "nyE3JKfAFUMS";
ESP8266WebServer server(80);

void callback(char *topic, byte *payload, unsigned int length) {
    if (String(topic).equals("/command")) {
        String msg = "";
        for (int i = 0; i < length; i++) {
            msg = msg + (char) payload[i];
        }
        Serial.println("received uid is :" + msg);
        client.publish("/command-response", msg.c_str());
    }
}

void mqttConnect() {
    while (!client.connected()) {
        client.setServer(mqttServer, mqttPort);
        client.setKeepAlive(15000);
        client.setCallback(callback);
        Serial.println("Connecting to MQTT...");
        if (client.connect("my-device", mqttUser, mqttPassword)) {
            Serial.println("connected");
            client.subscribe("/command");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.begin("", "");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Waiting for connection");
    }
    Serial.print("IP: ");
    Serial.println(WiFi.localIP().toString().c_str());
    mqttConnect();
    server.begin();
}

void loop() {
    server.handleClient();
    client.loop();
}