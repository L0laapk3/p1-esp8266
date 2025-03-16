#include <Arduino.h>
#ifdef OTA
#include <ArduinoOTA.h>
#endif
#include <ESP8266WiFi.h>
#include "secrets.h"

#ifndef SERIAL_RATE
#define SERIAL_RATE 115200
#endif

#ifndef SERIAL_BITS
#define SERIAL_BITS SERIAL_8N1
#endif


// Uncomment and fill in the next 4 lines to use a static IP address
// #define STATIC_IP
// IPAddress ip(192, 168, 0, 253);
// IPAddress gateway(192, 168, 0, 1);
// IPAddress subnet(255, 255, 255, 0);

WiFiServer server(23);
WiFiClient current_client;

#define IDEAL_PACKET_SIZE 1024
void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, !HIGH);
    // we start a serial, but invert it
    Serial.begin(SERIAL_RATE, SERIAL_BITS, SERIAL_RX_ONLY, 1, true);
    // we use the internal buffer of the serial class and send from that when we can
    // assuming we can send out TCP packets fast enough to not let it overflow
    Serial.setRxBufferSize(IDEAL_PACKET_SIZE * 8);
    Serial.setDebugOutput(false);

    String hostname = "ESP-P1-POWER-" + WiFi.macAddress(); // allow for multiple esp-p1-power on the network
    hostname.replace(":", "");
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.hostname(hostname.c_str());
#ifdef STATIC_IP
    WiFi.config(ip, gateway, subnet);
#endif
    WiFi.begin(WIFI_ACCESPOINT, WIFI_PASSWORD);
    server.begin();
    server.setNoDelay(true);

#ifdef OTA
    ArduinoOTA.setPassword("MONITOR_POWER_42");
    ArduinoOTA.setHostname(hostname.c_str());
    ArduinoOTA.begin();
#endif
}

unsigned long lastUpdate = 0;
#ifdef OTA
unsigned long lastOTACheck = 0;
#endif

void loop() {

    while (WiFi.status() != WL_CONNECTED) {
		delay(200);
		digitalWrite(LED_BUILTIN, !HIGH);
        delay(50);
		digitalWrite(LED_BUILTIN, !LOW);
    }

    auto now = millis();

    auto available = Serial.peekAvailable();
    if (available >= IDEAL_PACKET_SIZE || (available > 0 && (now - lastUpdate) > 50)) {
        auto buffer = Serial.peekBuffer();
        if (current_client && current_client.connected()) {
            auto written = current_client.write(buffer, available);
            Serial.peekConsume(written);
            lastUpdate = now;
            if (written > 0)
                digitalWrite(LED_BUILTIN, !HIGH);
        }
        else {
            Serial.peekConsume(available);
			delay(550);
			digitalWrite(LED_BUILTIN, !HIGH);
			delay(50);
			digitalWrite(LED_BUILTIN, !LOW);
        }
    }
    if (server.hasClient()) {
        current_client.stop();
        current_client = server.available();
    }
#ifdef OTA
    if (now - lastOTACheck > 1000) {
        ArduinoOTA.handle();
        lastOTACheck = now;
    }
    else {
        delay(10);
    }
#else
    delay(10);
#endif
}