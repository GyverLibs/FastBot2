#pragma once
#include <Arduino.h>
#include <Client.h>
#include <StringUtils.h>

#include "config.h"
#include "packet.h"

#define ASYNC_HTTP_DEF_TIMEOUT 2000
#define ASYNC_HTTP_BUF_SIZE 150

class AsyncHTTP : public Stream {
   public:
    enum class ContentType : uint8_t {
        None,
        Json,
        File,
    };

    AsyncHTTP(Client& client, const char* host, uint16_t port) : client(client), _host(host), _port(port) {
        setTimeout(ASYNC_HTTP_DEF_TIMEOUT);
    }
    AsyncHTTP(Client& client, const IPAddress& ip, uint16_t port) : client(client), _host(nullptr), _ip(ip), _port(port) {
        setTimeout(ASYNC_HTTP_DEF_TIMEOUT);
    }

    // ============== STREAM IMPL ==============
    // оставшийся content-length, если клиент ждёт ответа
    int available() {
        return isWaiting() ? _length : 0;
    }

    int read() {
        if (!client.available()) {
            uint32_t ms = millis();
            while (!client.available()) {
                if (millis() - ms >= _timeout) {
                    FB_LOG("client available timeout");
                    flush();
                    return -1;
                }
            }
        }

        int r = client.read();
        if (r >= 0) {
            if (_length) {
                _length--;
                if (!_length) flush();  // last
            } else {
                FB_LOG("client read error");
                flush();
                r = -1;
            }
        }
        return r;
    }

    int peek() {
        return client.peek();
    }

    size_t write(uint8_t) {
        return 1;
    }

    // ==========================

    // установить новый хост и порт
    void setHost(const char* host, uint16_t port) {
        if (client.connected()) {
            flush();
            stop();
        }
        _host = host;
        _port = port;
    }

    // установить новый хост и порт
    void setHost(const IPAddress& ip, uint16_t port) {
        setHost(nullptr, port);
        _ip = ip;
    }

    // установить новый клиент для связи
    void setClient(Client& client) {
        if (this->client.connected()) {
            flush();
            stop();
        }
        this->client = client;
        client.setTimeout(_timeout);
    }

    // установить таймаут ответа сервера, умолч. 2000 мс
    void setTimeout(uint16_t tout) {
        client.setTimeout(tout);
        _timeout = tout;
    }

    // ==========================

    // начать отправку. Дальше нужно вручную сделать client.print
    bool beginSend() {
        if (_wait) stop();

        if (!client.connected()) {
            if (_host) client.connect(_host, _port);
            else client.connect(_ip, _port);
        }

        if (client.connected()) {
            _wait = 1;
            return 1;
        } else {
            FB_LOG("client connect error");
            return 0;
        }
    }

    // синхронно дождаться ответа сервера
    bool waitAnswer() {
        if (!isWaiting()) return 0;

        client.flush();
        uint32_t ms = millis();
        while (!client.available()) {
            if (millis() - ms >= _timeout) {
                FB_LOG("client timeout error");
                flush();
                return 0;
            }
            yield();
        }
        return 1;
    }

    // парсить headers (по waitAnswer() или available())
    bool beginParse() {
        _length = 0;
        _close = 0;
        _type = ContentType::None;
        bool connF = 0, typeF = 0;
        bool eolF = 0;
        uint8_t buffer[ASYNC_HTTP_BUF_SIZE];

        while (client.connected()) {
            size_t len = client.readBytesUntil('\n', buffer, ASYNC_HTTP_BUF_SIZE);
            if (!len || buffer[len - 1] != '\r') break;
            if (len == 1) {  // line == \r
                eolF = 1;
                break;
            }

            su::Text line(buffer, len - 1);  // -\r
            int16_t colon = line.indexOf(':');
            if (colon > 0) {
                su::Text name = line.substring(0, colon);
                su::Text value = line.substring(colon + 2);  // ": "
                if (!_length && name == F("Content-Length")) {
                    _length = value;
                } else if (!connF && name == F("Connection")) {
                    connF = 1;
                    _close = (value == F("close"));
                } else if (!typeF && name == F("Content-Type")) {
                    typeF = 1;
                    if (value == F("application/json")) _type = ContentType::Json;
                    else if (value == F("application/octet-stream")) _type = ContentType::File;
                    else _type = ContentType::None;
                }
            }
            yield();
        }
        if (!eolF || !_length) {  // !eolF == error/disconnected
            FB_LOG("client headers error");
            flush();
            return 0;
        }
        return 1;
    }

    ContentType getType() {
        return _type;
    }

    // парсить пакет в буфер размера available()
    bool readBytes(char* buf) {
        bool ok = (client.readBytes(buf, _length) == _length);
        if (!ok) FB_LOG("client read error");
        flush();
        return ok;
    }

    // клиент ждёт ответа
    bool isWaiting() {
        return client.connected() && _wait;
    }

    // остановить клиента
    void stop() {
        FB_LOG("client stop");
        client.stop();
        _wait = 0;
        _length = 0;
    }

    // прочитать ответ, снять флаг ожидания, остановить если connection close
    void flush() {
        while (client.available()) {
            client.read();
            yield();
        }
        if (_close) stop();
        _wait = 0;
        _length = 0;
    }

    Client& client;

   private:
    const char* _host = nullptr;
    IPAddress _ip;
    uint16_t _port;
    bool _wait = 0;
    uint16_t _timeout;
    bool _close = 0;
    size_t _length = 0, _readLength = 0;
    ContentType _type = ContentType::None;
};