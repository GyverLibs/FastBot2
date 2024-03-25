#include <Arduino.h>

#define WIFI_SSID ""
#define WIFI_PASS ""
#define BOT_TOKEN ""
#define CHAT_ID ""

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include <FastBot2.h>
FastBot2 bot;

// обработчик сырого json пакета
void cbs(const String& s) {
    // Serial.println(s);
}

// обработчик ответов сервера
void cbr(gson::Entry& r) {
    // важный момент: отсюда нельзя ничего отправлять в бота! Библиотека проигнорирует отправку
    Serial.println(r[fbh::message_id]);
}

// обработчик обновлений
void cb(fb::Update& u) {
    if (u.isMessage()) {
        // Serial.println(u.message().date());
        Serial.println(u.message().text().toString(true));  // decode unicode
        // Serial.println(u.message().from().username());

        // эхо, вариант 1
        // fb::Message msg;
        // msg.text = u.message().text().;
        // msg.chat_id = u.message().chat().id();
        // bot.sendMessage(msg);

        // эхо, вариант 2
        bot.sendMessage(u.message().text(), u.message().chat().id());
    }

    // ответ на query
    if (u.isQuery()) {
        Serial.println(u.query().data());
        // bot.answerCallbackQuery(u.query().id());
        // bot.answerCallbackQuery(u.query().id(), "hello!");
        bot.answerCallbackQuery(u.query().id(), "hello!", true);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(20);
    Serial.println();

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");

    // attach
    bot.attachUpdate(cb);
    bot.attachResult(cbr);
    bot.attachRaw(cbs);

    // system
    bot.setToken(F(BOT_TOKEN));
    bot.skipUpdates();

    // выбор типа обновлений
    // bot.updates.clearAll();
    // bot.updates.set(fb::Updates::Type::message | fb::Updates::Type::channel_post);

    // настройка режима опроса
    // bot.setPollMode(FastBot2::Poll::Sync, 4000); // умолч
    // bot.setPollMode(FastBot2::Poll::Long, 10000);
    // bot.setPollMode(FastBot2::Poll::Async, 4000);

    // настройки сообщений по умолчанию
    // fb::Message::previewDefault = 1;
    // fb::Message::notificationDefault = 1;
    // fb::Message::protectDefault = 0;
    // fb::Message::modeDefault = fb::Message::Mode::Text;

    // настройки меню по умолчанию
    // fb::Menu::persistentDefault = 0;
    // fb::Menu::resizeDefault = 0;
    // fb::Menu::oneTimeDefault = 0;
    // fb::Menu::selectiveDefault = 0;
}

void loop() {
    bot.tick();

    if (Serial.available()) {
        // =============================
        // сообщение, вариант 1
        fb::Message msg(Serial.readString(), CHAT_ID);

        // =============================
        // сообщение, вариант 2
        // fb::Message msg;
        // msg.text = Serial.readString();
        // msg.chat_id = CHAT_ID;

        // =============================
        // меню, вариант 1
        // fb::Menu menu;
        // menu.text = "kek 1 ; kek 2 ; kek 3 \n kek 4 ; kek 5";
        // menu.resize = 1;
        // menu.placeholder = "Fancy placeholder";

        // =============================
        // меню, вариант 2
        fb::Menu menu;
        menu.addButton("kek 1").addButton("kek 2").newRow();
        menu.addButton("kek 3");

        // =============================
        // inline menu, вариант 1
        // fb::MenuInline menu("kek 1 ; kek 2 ; kek 3 \n kek 4 ; kek 5", "test;pest;lol;https://www.google.ru/;https://www.yandex.ru/");

        // =============================
        // inline menu, вариант 2
        // fb::MenuInline menu;
        // menu.addButton("BUTTON 1");  // data == text
        // menu.addButton("BUTTON 2", "data_2");
        // menu.newRow();
        // menu.addButton("BUTTON 3", "https://www.google.ru/");

        // =============================
        msg.setMenu(menu);

        bot.sendMessage(msg);
    }
}