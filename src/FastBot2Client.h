#pragma once
#include <Arduino.h>

#include "VirtualFastBot2.h"
#include "core/AsyncHTTP.h"
#include "core/config.h"

class FastBot2Client : public VirtualFastBot2 {
   public:
    FastBot2Client(Client& client) : VirtualFastBot2(), http(client, TELEGRAM_HOST, TELEGRAM_PORT) {}

    // установить таймаут ожидания ответа сервера (умолч. 2000 мс)
    void setTimeout(uint16_t timeout) {
        VirtualFastBot2::setTimeout(timeout);
        http.setTimeout(timeout);
    }

    // установить лимит памяти на ответ сервера (библиотека начнёт пропускать сообщения), умолч. 20000
    void setMemLimit(uint16_t limit) {
        _limit = limit;
    }

   private:
    AsyncHTTP http;
    uint16_t _limit = 20000;

    // override
    bool clientSend(fb::Packet& packet, bool wait) {
        while (http.isWaiting()) {
            _checkAndRead();
            yield();
        }
        return wait ? _sendAndWait(packet) : _send(packet);
    }

    bool clientWaiting() {
        return http.isWaiting();
    }

    void clientStop() {
        http.stop();
    }

    void clientTick() {
        _checkAndRead();
    }

    // func
    void _checkAndRead() {
        if (http.available() && http.beginParse()) _readAndParse();
    }
    bool _send(fb::Packet& packet) {
        if (http.beginSend()) {
            packet.printTo(http.client);
            return 1;
        }
        return 0;
    }
    bool _sendAndWait(fb::Packet& packet) {
        return _send(packet) && http.waitAnswer() && http.beginParse() && _readAndParse();
    }
    bool _readAndParse() {
        if (http.length() > _limit) {
            skipNextMessage();
            http.flush();
            return 0;
        }
        bool ok = 0;
        char* buf = new char[http.length()];
        if (buf) {
            if (http.parse(buf)) {
                su::Text txt(buf, http.length());
                ok = parsePacket(txt);
            }
            delete[] buf;
        }
        return ok;
    }
};