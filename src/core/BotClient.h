#pragma once
#include <Arduino.h>
#include <Client.h>

#include "config.h"
#include "packet.h"

namespace fb {

enum class Error : uint8_t {
    None,      // нет ошибки
    Timeout,   // таймаут
    Reserve,   // не получилось выделить память
    Length,    // несоответствие размера данных
    HTTP,      // ошибка HTTP пакета
    Client,    // ошибка клиента, не смог подключиться
    Telegram,  // ошибка телеграм (пакет не OK)
    MemLimit,  // слишком длинное сообщение
    Parse,     // ошибка парсинга пакета
};

}

class BotClient {
   public:
    BotClient(Client* client = nullptr) {
        setClient(client);
    }

    void setClient(Client* client) {
        _client = client;
        if (_client) _client->setTimeout(1000);
    }

    bool available() {
        return _client && _client->connected() && _client->available();
    }

    void setMemLimit(uint16_t limit) {
        _memlimit = limit;
    }

    bool send(fb::packet& data) {
        if (!_client) return 0;

        if (_wait_f) stop();
        last_ms = millis();
        if (!connect()) return 0;

        data.end();
        _client->print(data);
        _wait_f = 1;
        return 1;
    }

    String send_read(fb::packet& data, fb::Error* error = nullptr) {
        if (!_client) return String();

        bool sent = send(data);
        if (!sent) {
            if (error) *error = fb::Error::Client;
            return String();
        }

        _client->flush();
        uint32_t ms = millis();
        while (!available()) {
            if (millis() - ms >= timeout) {
                if (error) *error = fb::Error::Timeout;
                return String();
            }
            yield();
        }
        return read(error);
    }

    String read(fb::Error* error = nullptr) {
        if (!_client) return String();
        fb::Error err = fb::Error::None;
        int8_t close = 0;
        bool eol = 0;
        uint32_t len = 0;

        while (_client->connected()) {
            String line = _client->readStringUntil('\n');
            if (!close && line.indexOf(F("Connection: ")) > -1) close = line.indexOf(F("close"));
            else if (!len && line.indexOf(F("Content-Length: ")) > -1) len = atol(line.c_str() + 16);
            else if (line == "\r") {
                eol = 1;
                break;
            }
            yield();
        }

        String ret;
        if (eol && len) {
            if (len < _memlimit) {
                if (ret.reserve(len)) {
                    // parse while
                    uint32_t ms = millis();
                    while (_client->connected()) {
                        if (_client->available()) {
                            while (_client->available()) {
                                ret += (char)_client->read();
                                len--;
                                if (!len) break;
                            }
                            ms = millis();
                        }  // while available

                        if (millis() - ms >= timeout) {
                            err = fb::Error::Timeout;
                            len = 0;
                            break;
                        }
                        if (!len) break;
                        yield();
                    }  // while connected

                    if (len) err = fb::Error::Length;
                } else {
                    err = fb::Error::Reserve;
                }
            } else {
                err = fb::Error::MemLimit;
            }
        } else {
            err = fb::Error::HTTP;
        }

        // flush
        if (err != fb::Error::None) {
            while (_client->available()) {
                _client->read();
                yield();
            }
            ret = "";
        }

        _wait_f = 0;
        if (close > 0) stop();
        if (error) *error = err;
        return ret;
    }

    bool waiting() {
        return _wait_f;
    }

    void stop() {
        if (!_client) return;
        _client->stop();
        _wait_f = 0;
    }

    bool reconnect() {
        if (!_client) return 0;
        stop();
        return connect();
    }

    bool connected() {
        return _client ? _client->connected() : 0;
    }

    void setTimeout(uint16_t tout) {
        timeout = tout;
    }

    uint16_t timeout = 3000;
    uint32_t last_ms = 0;

   private:
    Client* _client = nullptr;
    bool _wait_f = 0;
    uint16_t _memlimit = 15000;

    bool connect() {
        if (!_client) return 0;
        if (!_client->connected()) {
            _client->connect(TELEGRAM_HOST, TELEGRAM_PORT);
        }
        return _client->connected();
    }
};

// bool send(const __FlashStringHelper* cmd, const String& data = "") {
//     if (!_client) return 0;
//     if (_wait_f) stop();
//     last_ms = millis();

//     if (!connect()) return 0;
//     String buf;
//     buf += F("POST /bot");
//     buf += token;
//     buf += '/';
//     buf += cmd;
//     buf += F(
//         " HTTP/1.1"
//         "\r\nHost: " TELEGRAM_HOST
//         "\r\nContent-Type: application/json"
//         "\r\nContent-Length: ");
//     buf += data.length() + 2;  // + {}
//     buf += F("\r\n\r\n{");
//     buf += data;
//     buf += '}';
//     _client->print(buf);
//     _wait_f = 1;
//     return 1;
// }

// String send_read(const __FlashStringHelper* cmd, const String& data = "", fb::Error* error = nullptr) {
//     if (!_client) return String();
//     bool sent = send(cmd, data);
//     if (!sent) {
//         if (error) *error = fb::Error::Client;
//         return String();
//     }
//     uint32_t ms = millis();
//     while (!available()) {
//         if (millis() - ms >= timeout) {
//             if (error) *error = fb::Error::Timeout;
//             return String();
//         }
//         yield();
//     }
//     return read(error);
// }