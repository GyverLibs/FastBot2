// ВНИМАНИЕ!
// это тестовый пример, я им тестирую функциональность и стабильность библиотеки
// тут расписана приличная часть возможностей вместе с тестами и комментариями

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

#include "image_p.h"
#include "lorem_p.h"

static char* __stack_start = nullptr;
void stackStart() {
    char c;
    __stack_start = &c;
}
void stackPrint() {
    char c;
    Serial.println(int32_t(__stack_start - &c));
}
static uint32_t _ms;
void loopStart() {
    _ms = millis();
}
void loopPrint() {
    Serial.print("exec. time: ");
    Serial.println(millis() - _ms);
}

#include <LittleFS.h>

// Нативная версия WiFi esp8266/esp32
#include <FastBot2.h>
FastBot2 bot;

// версия с другим клиентом, например tinygsm
// обязательно нужен модем с поддержкой HTTPS
// #define TINY_GSM_MODEM_SIM7000SSL
// #include <TinyGsmClient.h>
// TinyGsm modem(Serial);
// TinyGsmClient gsmclient(modem);
// #include <FastBot2Client.h>
// FastBot2Client bot(gsmclient);

void rawh(Text text);
void handleCommand(fb::Update& u);
void handleMessage(fb::Update& u);
void handleDocument(fb::Update& u);
void handleQuery(fb::Update& u);
void updateh(fb::Update& u);

void rawh(Text text) {
    // Serial.println(text);
}

void handleCommand(fb::Update& u) {
    Text chat_id = u.message().chat().id();

    switch (u.message().text().hash()) {
        case SH("/start"): {
            // установить команды
            // в конструкторе
            fb::MyCommands commands("help;settings", "Помощь;Настройки");

            // вручную
            // fb::MyCommands commands;
            // commands.commands = "help;settings";
            // commands.description = "Помощь;Настройки";

            // через билдер
            // fb::MyCommands commands;
            // commands.addCommand("help", "Помощь");
            // commands.addCommand("settings", "Настройки");
            FB_LOG("set cmds");
            bot.setMyCommands(commands);

            FB_LOG("set msg");

            fb::Message msg;
            msg.text = F(
                "Help:\r\n"
                "/start\r\n"
                "/send_msg\r\n"
                "/send_menu\r\n"
                "/send_inline_menu\r\n"
                "/file_txt\r\n"
                "/file_txt_p\r\n"
                "/file_txt_file_1kb\r\n"
                "/file_txt_file_50kb\r\n"
                "/file_img_p\r\n"
                "/file_img_url");
            msg.chatID = chat_id;
            bot.sendMessage(msg);
        } break;

        case SH("/send_msg"): {
            // вариант 1
            bot.sendMessage(fb::Message("Some text", chat_id));

            // вариант 2
            // fb::Message msg;
            // msg.text = "Send menu";
            // msg.chatID = chat_id;
            // bot.sendMessage(msg);
        } break;

        case SH("/send_menu"): {
            fb::Message msg("Send menu", chat_id);

            // меню, вариант 1
            // fb::Menu menu;
            // menu.text = "kek 1 ; kek 2 ; kek 3 \n kek 4 ; kek 5";
            // menu.resize = 1;
            // menu.placeholder = "Fancy placeholder";
            // msg.setMenu(menu);

            // меню, вариант 2
            fb::Menu menu;
            menu.addButton("kek 1").addButton("kek 2").newRow();
            menu.addButton("kek 3");
            msg.setMenu(menu);

            bot.sendMessage(msg);
        } break;

        case SH("/send_inline_menu"): {
            fb::Message msg("Send inline menu", chat_id);

            // inline menu, вариант 1
            // fb::InlineMenu menu("kek 1 ; kek 2 ; kek 3 \n kek 4 ; kek 5", "test;pest;lol;https://www.google.ru/;https://www.yandex.ru/");
            // msg.setInlineMenu(menu);

            // inline menu, вариант 2
            fb::InlineMenu menu;
            menu.addButton("BUTTON 1");  // data == text
            menu.addButton("BUTTON 2", "data_2");
            menu.newRow();
            menu.addButton("BUTTON 3", "https://www.google.ru/");
            menu.newRow();
            menu.addButton("Change message", "change_msg");
            menu.addButton("Change menu", "change_menu");
            msg.setInlineMenu(menu);

            bot.sendMessage(msg);
        } break;

        case SH("/file_txt"): {
            char str[] = "hello text v1";
            fb::File f("file.txt", fb::File::Type::document, (uint8_t*)str, strlen(str));
            f.chatID = chat_id;
            bot.sendFile(f);
            // delay(5000);
            // {
            //     char str[] = "hello text v2";
            //     fb::FileEdit f("file.txt", fb::File::Type::document, (uint8_t*)str, strlen(str));
            //     f.messageID = bot.lastBotMessage();
            //     f.chatID = chat_id;
            //     bot.editFile(f);
            // }
        } break;

        case SH("/file_txt_p"): {
            fb::File f("file.txt", fb::File::Type::document, (const uint8_t*)lorem_p, sizeof(lorem_p), true);
            f.chatID = chat_id;
            loopStart();
            bot.sendFile(f, false);
            loopPrint();
            // delay(5000);
            // {
            //     char str[] = "hello text v2";
            //     fb::FileEdit f("file.txt", fb::File::Type::document, (uint8_t*)str, strlen(str));
            //     f.messageID = bot.lastBotMessage();
            //     f.chatID = chat_id;
            //     bot.editFile(f);
            // }
        } break;

        case SH("/file_txt_file_1kb"): {
            File file = LittleFS.open("/lorem_1kb.txt", "r");
            fb::File f("file_1kb.txt", fb::File::Type::document, file);
            f.chatID = chat_id;
            loopStart();
            bot.sendFile(f, false);
            loopPrint();
        } break;

        case SH("/file_txt_file_50kb"): {
            File file = LittleFS.open("/lorem_50kb.txt", "r");
            fb::File f("file_50kb.txt", fb::File::Type::document, file);
            f.chatID = chat_id;
            loopStart();
            bot.sendFile(f, false);
            loopPrint();
        } break;

        case SH("/file_img_p"): {
            fb::File f("bot.png", fb::File::Type::photo, image_p, sizeof(image_p), true);
            f.chatID = chat_id;
            loopStart();
            bot.sendFile(f, false);
            loopPrint();
        } break;

        case SH("/file_img_url"): {
            fb::File f("file.txt", fb::File::Type::document, "https://upload.wikimedia.org/wikipedia/ru/6/61/Rickrolling.gif");
            f.chatID = chat_id;
            loopStart();
            bot.sendFile(f, false);
            loopPrint();
            // delay(2000);
            // {
            //     fb::FileEdit f("file.txt", fb::File::Type::document, "https://user-images.githubusercontent.com/14011726/94132137-7d4fc100-fe7c-11ea-8512-69f90cb65e48.gif");
            //     f.messageID = bot.lastBotMessage();
            //     f.chatID = chat_id;
            //     bot.editFile(f);
            // }
        } break;
    }
}

void handleMessage(fb::Update& u) {
    if (u.isMessage()) {
        // Serial.println(u.message().date());
        // Serial.println(u[tg_apih::text]);
        // Serial.println(u.message().text().decodeUnicode());
        Serial.println(u.message().text());
        Serial.println(u.message().from().username());
        Serial.println(u.message().from().id());

        // handleCommand
        if (u.message().text().startsWith('/')) {
            handleCommand(u);
        } else {
            // эхо, вариант 1
            // fb::Message msg;
            // msg.text = u.message().text().toString();
            // msg.chatID = u.message().chat().id();
            // bot.sendMessage(msg, false);

            // эхо, вариант 2
            bot.sendMessage(fb::Message(u.message().text().toString(), u.message().chat().id()));

            // ============================
            // удалить сообщение юзера
            // bot.deleteMessage(u.message().chat().id(), u.message().id());
        }

        // ============================
        // изменить последнее сообщение на текст из чата
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
void handleDocument(fb::Update& u) {
    if (u.message().document().name().endsWith(".bin")) {  // .bin - значит это ОТА
        bot.sendMessage(fb::Message("OTA begin", u.message().chat().id()), true);

        // между downloadFile и updateFlash/updateFS/writeTo не должно быть отправки сообщений!
        // OTA обновление тип 1
        bot.updateFlash(u.message().document(), u.message().chat().id());

        // OTA обновление тип 2
        // fb::Fetcher fetch = bot.downloadFile(u.message().document().id());
        // if (fetch) {
        //     if (fetch.updateFlash()) {
        //         Serial.println("OTA done");
        //         bot.sendMessage(fb::Message("OTA done", u.message().chat().id()), true);
        //     } else {
        //         Serial.println("OTA error");
        //         bot.sendMessage(fb::Message("OTA error", u.message().chat().id()), true);
        //     }
        // }
    } else {
        // это просто файл, выведем содержимое
        fb::Fetcher fetch = bot.downloadFile(u.message().document().id());
        if (fetch) {
            // вывести в сериал
            Serial.println(fetch);

            // записать в файл
            // File file = LittleFS.open("file.txt", "w");
            // f.writeTo(file);
        }
    }
}
void handleQuery(fb::Update& u) {
    fb::QueryRead q = u.query();
    Serial.println(q.data());

    // ответ
    // bot.answerCallbackQuery(q.id());
    bot.answerCallbackQuery(q.id(), q.data());
    // bot.answerCallbackQuery(q.id(), q.data(), true);

    if (q.data() == "change_msg") {
        fb::TextEdit t;
        t.text = "New message + new menu";
        t.chatID = q.message().chat().id();
        t.messageID = q.message().id();
        fb::InlineMenu menu("kek 1;kek 2;kek 3", "1;2;3");
        t.setInlineMenu(menu);
        bot.editText(t);
    } else if (q.data() == "change_menu") {
        fb::MenuEdit m;
        m.chatID = q.message().chat().id();
        m.messageID = q.message().id();
        fb::InlineMenu menu("pepe 1;pepe 2;pepe 3", "11;22;33");
        m.setInlineMenu(menu);
        bot.editMenu(m);
    }
}

// обработчик обновлений
void updateh(fb::Update& u) {
    // bot.sendMessage(fb::Message("Some text", CHAT_ID));
    // return;

    // разбил на функции, чтобы не переполнять стек esp8266!
    // uint32_t heap = ESP.getFreeHeap();

    if (u.isMessage()) handleMessage(u);
    if (u.isMessage() && u.message().hasDocument()) handleDocument(u);
    if (u.isQuery()) handleQuery(u);

    // =================== CUSTOM ===================
    // полное определение типа обновления:
    switch (u.type()) {
        case fb::Update::Type::Message:
            break;
        case fb::Update::Type::ChatJoinRequest:
            break;
        default:
            break;
    }

    // доступ к json пакету
    // Например для сообщения:
    // Serial.println(u[tg_apih::from][tg_apih::username]);

    // ============================
    // service
    // if (heap != ESP.getFreeHeap()) {
    //     Serial.println("MEMORY LEAK!!!!!!!!!!!!");
    //     Serial.print(heap);
    //     Serial.print("->");
    //     Serial.println(ESP.getFreeHeap());
    // }
    // Serial.print("stack: ");
    // stackPrint();
    Serial.print("heap: ");
    Serial.println(ESP.getFreeHeap());
}

void setup() {
    stackStart();

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
    bot.attachUpdate(updateh);
    bot.attachRaw(rawh);
    // bot.attachResult(responseh);

    // system
    bot.setToken(F(BOT_TOKEN));
    bot.skipUpdates();

#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif

    if (!LittleFS.exists("/lorem_1kb.txt")) {
        File f = LittleFS.open("/lorem_1kb.txt", "w");
        f.print(FPSTR(lorem_p));
    }
    if (!LittleFS.exists("/lorem_50kb.txt")) {
        File f = LittleFS.open("/lorem_50kb.txt", "w");
        for (int i = 0; i < 50; i++) f.println(FPSTR(lorem_p));
    }

    // ============================
    // выбор типа обновлений
    // bot.updates.clearAll();
    // bot.updates.set(fb::Updates::Type::Message | fb::Updates::Type::ChannelPost);

    // ============================
    // настройка режима опроса
    // bot.setPollMode(fb::Poll::Sync, 4000);  // умолч
    // bot.setPollMode(fb::Poll::Async, 4000);
    bot.setPollMode(fb::Poll::Long, 20000);

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
    // fb::Packet p = bot.beginPacket(tg_cmd::sendMessage);  // Все команды API Telegram доступны в tg_cmd

    // p.addString(tg_api::text, "message text");  // все ключи объектов API Telegram доступны в tg_api
    // p.addInt(tg_api::chat_id, CHAT_ID);
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
    // отправка кастомной команды из gson::string
    // gson::string g;
    // g.beginObj();
    // g[tg_api::chat_id] = CHAT_ID;
    // g[tg_api::text] = "hello text";
    // g.endObj();
    // g.end();
    // fb::Result res = bot.sendCommand(tg_cmd::sendMessage, g);
    // res.stringify(Serial);

    // ============================
    // отправка команды без параметров
    // fb::Result res = bot.sendCommand(tg_cmd::getMe);
    // Serial.println(res[tg_apih::first_name]);
    // Serial.println(res[tg_apih::username]);
    // Serial.println(res.getRaw());
}

void loop() {
    uint32_t ms = millis();

    // тикаем в loop!
    bot.tick();

    if (bot.canReboot()) ESP.restart();

    ms = millis() - ms;
    if (ms > 100) Serial.println(String("--------- looptime: ") + ms);
}