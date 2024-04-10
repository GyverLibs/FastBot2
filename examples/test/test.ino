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

#include <LittleFS.h>

// Нативная версия WiFi esp8266/esp32
#include <FastBot2.h>
FastBot2 bot;

// версия с другим клиентом, например tinygsm
// #define TINY_GSM_MODEM_SIM800
// #include <TinyGsmClient.h>
// TinyGsm modem(Serial);
// TinyGsmClient gsmclient(modem);
// #include <FastBot2Client.h>
// FastBot2Client bot(gsmclient);

// обработчик сырого json String пакета
void rawh(const su::Text& s) {
    // Serial.println(s);
}

// обработчик скачивания файлов (байтовый поток) из getFile
void fetchh(Stream& s) {
    Serial.println("got file:");
    while (s.available()) Serial.print((char)s.read());
    Serial.println();
}

// обработчик ответов сервера
void responseh(gson::Entry& r) {
    // сюда прилетает разобранный json объект "result:{}" с хешированными ключами!
    // для доступа использовать хэши API, все доступны в fbh::api
    // Serial.println(r[fbh::api::message_id]);
    // Serial.println(r[fbh::api::text]);
    // Serial.println(r[fbh::api::from][fbh::api::username]);

    // можно узнать последнюю отправленную команду (результат её выполнения как раз в ответе сервера)
    switch (bot.lastCmd()) {
        case fbh::cmd::sendMessage:
            Serial.println("sendMessage");
            break;
        case fbh::cmd::sendDocument:
            Serial.println("sendDocument");
            break;
        case fbh::cmd::editMessageMedia:
            Serial.println("editMessageMedia");
            break;
    }

    // вывести в serial как json
    // r.stringify(Serial);
}

void foo1(fb::Update& u) {
    if (u.isMessage()) {
        Serial.println(u.message().date());
        Serial.println(u.message().text());
        Serial.println(u.message().text().toString(true));  // decode unicode
        Serial.println(u.message().from().username());

        // эхо, вариант 1
        // fb::Message msg;
        // msg.text = (String)u.message().text();
        // msg.chatID = u.message().chat().id();
        // bot.sendMessage(msg);

        // эхо, вариант 2
        bot.sendMessage(fb::Message(u.message().text().toString(true), u.message().chat().id()));

        // удалить сообщение юзера
        // bot.deleteMessage(u.message().chat().id(), u.message().id());

        // edit
        // if (bot.lastBotMessage()) {
        //     fb::TextEdit et;
        //     et.text = u.message().text().toString();
        //     et.chatID = u.message().chat().id();
        //     et.messageID = bot.lastBotMessage();
        //     bot.editText(et);
        // } else {
        //     bot.sendMessage(fb::Message(u.message().text(), u.message().chat().id()));
        // }
    }
}
void foo2(fb::Update& u) {
    if (u.isMessage() && u.message().hasDocument()) {
        if (u.message().document().name().endsWith(".bin")) {
            // .bin - значит это ОТА
            bot.sendMessage(fb::Message("OTA begin", u.message().chat().id()), true);

            // между downloadFile и updateFlash/updateFS/writeTo не должно быть отправки сообщений!
            fb::Fetcher fetch = bot.downloadFile(u.message().document().id());
            if (fetch.updateFlash()) {
                Serial.println("OTA done");
                bot.sendMessage(fb::Message("OTA done", u.message().chat().id()), true);
            } else {
                Serial.println("OTA error");
                bot.sendMessage(fb::Message("OTA error", u.message().chat().id()), true);
            }
        } else {
            // это просто файл, выведем содержимое
            fb::Fetcher fetch = bot.downloadFile(u.message().document().id());
            // вывести в сериал
            fetch.writeTo(Serial);
            Serial.println();

            // записать в файл
            // File file = LittleFS.open("file.txt", "w");
            // f.writeTo(file);
        }
    }
}
void foo3(fb::Update& u) {
    if (u.isQuery()) {
        Serial.println(u.query().data());

        // ответ
        // bot.answerCallbackQuery(u.query().id());
        // bot.answerCallbackQuery(u.query().id(), "hello!");
        bot.answerCallbackQuery(u.query().id(), "hello!", true, true);
    }
}
void foo4(fb::Update& u) {
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
    // Serial.println(u.entry[fbh::api::from][fbh::api::username]);
}

// обработчик обновлений
void updateh(fb::Update& u) {
    // разбил на функции, чтобы не переполнять стек esp8266!
    foo1(u);
    foo2(u);
    foo3(u);
    foo4(u);
}

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(20);
    Serial.println();
    Serial.println("version 2");

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");

    // attach
    bot.attachUpdate(updateh);
    bot.attachResult(responseh);
    bot.attachRaw(rawh);
    bot.attachFetch(fetchh);

    // system
    bot.setToken(F(BOT_TOKEN));
    bot.skipUpdates();

    // у 8266 можно уменьшить буфер клиента до минимума, его вполне хватает
    bot.client.setBufferSizes(512, 512);

    // ============================
    // выбор типа обновлений
    // bot.updates.clearAll();
    // bot.updates.set(fb::Updates::Type::Message | fb::Updates::Type::ChannelPost);

    // ============================
    // настройка режима опроса
    // bot.setPollMode(fb::Poll::Sync, 4000);  // умолч
    // bot.setPollMode(fb::Poll::Async, 4000);
    bot.setPollMode(fb::Poll::Long, 10000);

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
    // fb::Packet p = bot.beginPacket(fb::cmd::sendMessage);  // Все команды API Telegram доступны в fb::cmd

    // p.addString(fb::api::text, "message text");  // все ключи объектов API Telegram доступны в fb::api
    // p.addInt(fb::api::chat_id, CHAT_ID);
    // bot.sendPacket(p);
    // таким образом можно отправить любой API запрос

    // ============================
    // отправка нескольким ID
    // fb::Message msg;
    // msg.text = "hello!";

    // su::TextParser ids("546343285;1234853;8796453678;38347567", ';');
    // while (ids.parse()) {
    //     msg.chatID = ids;
    //     bot.sendMessage(msg);
    // }

    // ============================
    // send+edit url gif
    // fb::File f("file.txt", fb::File::Type::document, "https://compote.slate.com/images/697b023b-64a5-49a0-8059-27b963453fb1.gif");
    // f.chatID = CHAT_ID;
    // bot.sendFile(f, true);
    // delay(2000);
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

    // file from fs
    // LittleFS.begin();
    // File file = LittleFS.open("/image.jpg", "r");
    // fb::File f("file.txt", fb::File::Type::photo, file);
    // f.chatID = CHAT_ID;
    // bot.sendFile(f, true);

    // bot.sendMessage(fb::Message("Привет", CHAT_ID));

    // setMyCommands v1
    // fb::Packet p = bot.beginPacket(fb::cmd::setMyCommands);
    // p.beginArr(fb::api::commands);
    // p.addText(R"(
    //     {"command":"help","description":"Помощь по командам"},
    //     {"command":"info","description":"Информация о настройках"},
    //     {"command":"status","description":"Показания датчиков"},
    //     {"command":"restart","description":"Перезагрзка контроллера"}
    // )");
    // p.endArr();
    // bot.sendPacket(p);

    // setMyCommands v2
    // fb::Packet p = bot.beginPacket(fb::cmd::setMyCommands);
    // p.beginArr(fb::api::commands);
    // p.beginObj().addString(fb::api::command, "help").addString(fb::api::description, "Помощь по командам").endObj();
    // p.beginObj().addString(fb::api::command, "info").addString(fb::api::description, "Информация о настройках").endObj();
    // p.endArr();
    // bot.sendPacket(p);
}

void loop() {
    // тикаем в loop!
    bot.tick();
    if (bot.canReboot()) ESP.restart();

    // отправка сообщения текстом из Serial
    if (Serial.available()) {
        // сообщение, вариант 1
        fb::Message msg(Serial.readString(), CHAT_ID);

        // =============================
        // сообщение, вариант 2
        // fb::Message msg;
        // msg.text = Serial.readString();
        // msg.chatID = CHAT_ID;

        // =============================
        // меню, вариант 1
        // fb::Menu menu;
        // menu.text = "kek 1 ; kek 2 ; kek 3 \n kek 4 ; kek 5";
        // menu.resize = 1;
        // menu.placeholder = "Fancy placeholder";
        // msg.setMenu(menu);

        // =============================
        // меню, вариант 2
        // fb::Menu menu;
        // menu.addButton("kek 1").addButton("kek 2").newRow();
        // menu.addButton("kek 3");
        // msg.setMenu(menu);

        // =============================
        // inline menu, вариант 1
        fb::InlineMenu menu("kek 1 ; kek 2 ; kek 3 \n kek 4 ; kek 5", "test;pest;lol;https://www.google.ru/;https://www.yandex.ru/");
        msg.setInlineMenu(menu);

        // =============================
        // inline menu, вариант 2
        // fb::InlineMenu menu;
        // menu.addButton("BUTTON 1");  // data == text
        // menu.addButton("BUTTON 2", "data_2");
        // menu.newRow();
        // menu.addButton("BUTTON 3", "https://www.google.ru/");
        // msg.setInlineMenu(menu);

        bot.sendMessage(msg);
    }
}