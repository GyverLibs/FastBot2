#pragma once
#include <Arduino.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <WiFiClientSecure.h>
#endif

#include "FastBot2Client.h"

class FastBot2 : public FastBot2Client {
   public:
    FastBot2() : FastBot2Client(client) {
        client.setInsecure();
#if defined(ESP8266)
        client.setBufferSizes(512, 512);
#endif
    }

#if defined(ESP8266)
    BearSSL::WiFiClientSecure client;
#elif defined(ESP32)
    WiFiClientSecure client;
#endif
   private:
};