#include <Arduino.h>

#define WIFI_SSID ""
#define WIFI_PASS ""
#define BOT_TOKEN ""
#define CHAT_ID ""

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
BearSSL::WiFiClientSecure client;
#else  // ESP32
#include <WiFi.h>
#include <WiFiClientSecure.h>
WiFiClientSecure client;
#endif

#include <FastBot2.h>
FastBot2 bot(&client);

void cb(fb::Update& u) {
    if (u.isMessage()) {
        Serial.println(u.message().date());
        Serial.println(u.message().text());
        Serial.println(u.message().from().username());

        // эхо
        fb::Message msg;
        msg.text = u.message().text().str();
        msg.chat_id = u.message().chat().id();
        bot.sendMessage(msg);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");

    client.setInsecure();
    // client.setBufferSizes(512, 512);

    bot.setToken(F(BOT_TOKEN));
    bot.attach(cb);

    // bot.updates.clearAll();
    // bot.updates.set(fb::Updates::Type::message | fb::Updates::Type::channel_post);
    // bot.setPollMode(FastBot2::Poll::Long, 10000);
    // bot.setPollMode(FastBot2::Poll::Async, 3000);

    Serial.setTimeout(20);
}

void loop() {
    bot.tick();

    if (Serial.available()) {
        fb::Message msg;
        msg.text = Serial.readString();
        msg.chat_id = CHAT_ID;
        bot.sendMessage(msg);
    }
}