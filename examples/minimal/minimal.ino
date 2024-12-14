#include <Arduino.h>

#define WIFI_SSID ""
#define WIFI_PASS ""
#define BOT_TOKEN ""
#define CHAT_ID ""

#include <FastBot2.h>
FastBot2 bot;

void updateh(fb::Update& u) {
    Serial.println("NEW MESSAGE");
    Serial.println(u.message().from().username());
    Serial.println(u.message().text());

    // #1
    // отправить обратно в чат (эхо)
    bot.sendMessage(fb::Message(u.message().text(), u.message().chat().id()));

    // #2
    // декодирование Unicode символов (кириллицы) делается вручную!
    // String text = u.message().text().decodeUnicode();
    // text += " - ответ";
    // bot.sendMessage(fb::Message(text, u.message().chat().id()));

    // #3
    // или так
    // fb::Message msg;
    // msg.text = u.message().text().toString();
    // msg.chatID = u.message().chat().id();
    // bot.sendMessage(msg);
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

    // поприветствуем админа
    bot.sendMessage(fb::Message("Hello!", CHAT_ID));
}

void loop() {
    // вызывать тикер в loop
    bot.tick();
}