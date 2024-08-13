#include <Arduino.h>

#define WIFI_SSID ""
#define WIFI_PASS ""
#define BOT_TOKEN ""
#define CHAT_ID ""

#include <FastBot2.h>
FastBot2 bot;

void updateh(fb::Update& u) {
    if (u.isQuery()) {
        Serial.println("NEW QUERY");
        Serial.println(u.query().data());

        // ответ на query
        // bot.answerCallbackQuery(u.query().id());
        bot.answerCallbackQuery(u.query().id(), "query answered");
        // bot.answerCallbackQuery(u.query().id(), u.query().data(), true);

        // реакция на query. Для удобства обработаем через хэш
        switch (u.query().data().hash()) {
            case "test"_h:
                // кнопка kek1
                Serial.println("включить светодиод");
                break;

            case "pest"_h:
                // кнопка kek2
                break;

            case "lol"_h:
                // кнопка kek3
                break;
        }
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

    // ============
    bot.attachUpdate(updateh);   // подключить обработчик обновлений
    bot.setToken(F(BOT_TOKEN));  // установить токен

    // режим опроса обновлений. Самый быстрый - Long
    // особенности читай тут в самом низу
    // https://github.com/GyverLibs/FastBot2/blob/main/docs/3.start.md

    // bot.setPollMode(fb::Poll::Sync, 4000);  // умолч
    // bot.setPollMode(fb::Poll::Async, 4000);
    bot.setPollMode(fb::Poll::Long, 20000);

    fb::Message msg("Send inline menu", CHAT_ID);
    fb::InlineMenu menu("kek 1 ; kek 2 ; kek 3 \n kek 4 ; kek 5", "test;pest;lol;https://www.google.ru/;https://www.yandex.ru/");
    msg.setInlineMenu(menu);
    bot.sendMessage(msg);
}

void loop() {
    // вызывать тикер в loop
    bot.tick();
}