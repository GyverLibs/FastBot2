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
        if (http.isWaiting()) FB_LOG("client wait");

        while (http.isWaiting()) {
            _checkAndRead();
            yield();
        }
        return wait ? _sendAndWait(packet) : _send(packet);
    }

    bool clientStream(fb::Packet& packet, Stream** stream) {
        while (http.isWaiting()) {
            _checkAndRead();
            yield();
        }

        if (_send(packet) && http.waitAnswer() && http.beginParse()) {
            if (http.getType() == AsyncHTTP::ContentType::File) {
                *stream = &http;
                return 1;
            } else {
                FB_LOG("content type error");
                http.flush();
                return 0;
            }
        }
        FB_LOG("send fetch error");
        return 0;
    }

    bool clientWaiting() {
        return http.isWaiting();
    }

    void clientStop() {
        http.stop();
    }

    bool clientTick() {
        return _checkAndRead();
    }

    // func
    bool _checkAndRead() {
        if (http.clientAvailable()) {
            if (http.beginParse()) {
                return _readAndParse();
            } else {
                FB_LOG("begin parse error");
                return 0;
            }
        }
        return 0;
    }
    bool _send(fb::Packet& packet) {
        if (http.beginSend()) {
            packet.printTo(http.client);
            return 1;
        }
        FB_LOG("send error");
        return 0;
    }
    bool _sendAndWait(fb::Packet& packet) {
        return _send(packet) && http.waitAnswer() && http.beginParse() && _readAndParse();
    }
    bool _readAndParse() {
        // headers уже прочитаны тут
        switch (http.getType()) {
            case AsyncHTTP::ContentType::Json:
                if (_limit && http.available() > _limit) {
                    FB_LOG("memory limit error");
                    skipNextMessage();
                    http.flush();
                    return 0;
                } else {
                    bool ok = 0;
                    size_t len = http.available();
                    char* buf = new char[len];
                    if (buf) {
                        if (http.parse(buf)) {
                            su::Text txt(buf, len);
                            ok = parsePacket(txt);
                        }
                        delete[] buf;
                    }
                    return ok;
                }

            case AsyncHTTP::ContentType::File:
                handleFile(http);
                http.flush();
                return 1;

            default:
                FB_LOG("content type error");
                http.flush();
                return 0;
        }
        return 0;
    }
};