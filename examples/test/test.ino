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
// FastBot2 bot(&myClient); // можно указать другой Client для связи

// например tinygsm
// #define TINY_GSM_MODEM_SIM800
// #include <TinyGsmClient.h>
// TinyGsm modem(Serial);
// TinyGsmClient gsmclient(modem);
// FastBot2 bot(&gsmclient);

// обработчик сырого json String пакета
void cbs(const String& s) {
    Serial.println(s);
}

// обработчик ответов сервера
void cbr(gson::Entry& r) {
    // =================== CUSTOM ===================
    // сюда прилетает разобранный json объект "result:{}" с хешированными ключами!
    // для доступа использовать хэши API, все доступны в fbh
    // Serial.println(r[fbh::message_id]);
    // Serial.println(r[fbh::text]);
    // Serial.println(r[fbh::from][fbh::username]);
}

// обработчик обновлений
void cb(fb::Update& u) {
    // сообщение
    if (u.isMessage()) {
        // Serial.println(u.message().date());
        Serial.println(u.message().text());
        Serial.println(u.message().text().toString(true));  // decode unicode
        // Serial.println(u.message().from().username());

        // эхо, вариант 1
        // fb::Message msg;
        // msg.text = u.message().text();
        // msg.chatId = u.message().chat().id();
        // bot.sendMessage(msg);

        // эхо, вариант 2
        // bot.sendMessage(u.message().text(), u.message().chat().id());
        // bot.deleteMessage(u.message().chat().id(), u.message().id());

        // edit
        // if (bot.lastBotMessage()) {
        //     fb::EditText et;
        //     et.text = u.message().text().toString();
        //     et.chatID = u.message().chat().id();
        //     et.messageID = bot.lastBotMessage();
        //     bot.editText(et);
        // } else {
        //     bot.sendMessage(u.message().text(), u.message().chat().id());
        // }
    }

    // ответ на query
    if (u.isQuery()) {
        Serial.println(u.query().data());

        // ответ
        // bot.answerCallbackQuery(u.query().id());
        // bot.answerCallbackQuery(u.query().id(), "hello!");
        bot.answerCallbackQuery(u.query().id(), "hello!", true);
    }

    // =================== CUSTOM ===================
    // полное определение типа обновления:
    switch (u.type()) {
        case fb::Update::Type::message:
            break;
        case fb::Update::Type::chatJoinRequest:
            break;
        default:
            break;
    }

    // доступ к json пакету, здесь u.entry - api объект типа u.type()
    // Например для сообщения:
    Serial.println(u.entry[fbh::from][fbh::username]);
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

    // ============================
    // выбор типа обновлений
    // bot.updates.clearAll();
    // bot.updates.set(fb::Updates::Type::message | fb::Updates::Type::channel_post);

    // ============================
    // настройка режима опроса
    // bot.setPollMode(FastBot2::Poll::Sync, 4000); // умолч
    // bot.setPollMode(FastBot2::Poll::Async, 4000);
    // bot.setPollMode(FastBot2::Poll::Long, 10000);

    // ============================
    // настройки сообщений по умолчанию
    // fb::Message::previewDefault = 1;
    // fb::Message::notificationDefault = 1;
    // fb::Message::protectDefault = 0;
    // fb::Message::modeDefault = fb::Message::Mode::Text;

    // ============================
    // настройки меню по умолчанию
    // fb::Menu::persistentDefault = 0;
    // fb::Menu::resizeDefault = 0;
    // fb::Menu::oneTimeDefault = 0;
    // fb::Menu::selectiveDefault = 0;

    // ============================
    // отправка сообщения вручную. Начнём с команды
    // fb::Packet p = bot.beginPacket(F("sendMessage"));   // как F-строка
    fb::Packet p = bot.beginPacket(fbcmd::sendMessage());  // Все команды API Telegram доступны в fbcmd

    p.addString(fbapi::text(), "message text");  // все ключи объектов API Telegram доступны в fbapi
    p.addInt(fbapi::chat_id(), CHAT_ID);
    // bot.sendPacket(p);
    // таким образом можно отправить любой API запрос

    // send+edit url gif
    // fb::File f("file.txt", fb::File::Type::document, "https://compote.slate.com/images/697b023b-64a5-49a0-8059-27b963453fb1.gif");
    // f.chatID = CHAT_ID;
    // bot.sendFile(f, true);
    // delay(1000);
    // {
    //     fb::FileEdit f("file.txt", fb::File::Type::document, "https://user-images.githubusercontent.com/14011726/94132137-7d4fc100-fe7c-11ea-8512-69f90cb65e48.gif");
    //     f.messageID = bot.lastBotMessage();
    //     f.chatID = CHAT_ID;
    //     bot.editFile(f, true);
    // }

    // send+edit file
    // char str[] = "hello text v1";
    // fb::File f("file.txt", fb::File::Type::document, (uint8_t*)str, strlen(str));
    // f.chatID = CHAT_ID;
    // bot.sendFile(f, true);
    // delay(5000);
    // {
    //     char str[] = "hello text v2";
    //     fb::FileEdit f("file.txt", fb::File::Type::document, (uint8_t*)str, strlen(str));
    //     f.messageID = bot.lastBotMessage();
    //     f.chatID = CHAT_ID;
    //     bot.editFile(f);
    // }
}

void loop() {
    // тикаем в loop!
    bot.tick();

    // отправка сообщения текстом из Serial
    if (Serial.available()) {
        // сообщение, вариант 1
        fb::Message msg(Serial.readString(), CHAT_ID);

        // =============================
        // сообщение, вариант 2
        // fb::Message msg;
        // msg.text = Serial.readString();
        // msg.chatId = CHAT_ID;

        // =============================
        // меню, вариант 1
        // fb::Menu menu;
        // menu.text = "kek 1 ; kek 2 ; kek 3 \n kek 4 ; kek 5";
        // menu.resize = 1;
        // menu.placeholder = "Fancy placeholder";

        // =============================
        // меню, вариант 2
        // fb::Menu menu;
        // menu.addButton("kek 1").addButton("kek 2").newRow();
        // menu.addButton("kek 3");

        // =============================
        // inline menu, вариант 1
        fb::MenuInline menu("kek 1 ; kek 2 ; kek 3 \n kek 4 ; kek 5", "test;pest;lol;https://www.google.ru/;https://www.yandex.ru/");

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