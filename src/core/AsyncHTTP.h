#pragma once
#include <Arduino.h>
#include <Client.h>

#include "config.h"
#include "packet.h"

#define ASYNC_HTTP_DEF_TIMEOUT 2000

class AsyncHTTP {
   public:
    AsyncHTTP(Client& client, const char* host, uint16_t port) : client(client), _host(host), _port(port) {
        setTimeout(ASYNC_HTTP_DEF_TIMEOUT);
    }

    // установить таймаут ответа сервера, умолч. 2000 мс
    void setTimeout(uint16_t tout) {
        client.setTimeout(tout);
        _timeout = tout;
    }

    // начать отправку. Дальше нужно вручную сделать client.print
    bool beginSend() {
        if (_wait_f) stop();  // принудительно завершить прошлый сеанс
        if (!client.connected()) client.connect(_host, _port);
        _wait_f = client.connected();
        return _wait_f;
    }

    // асинхронно, клиент имеет входящие данные
    bool available() {
        return _wait_f && client.connected() && client.available();
    }

    // синхронно, дождаться ответа сервера
    bool waitAnswer() {
        if (!_wait_f) return 0;

        client.flush();
        uint32_t ms = millis();
        while (!available()) {
            if (millis() - ms >= _timeout) return 0;
            yield();
        }
        return 1;
    }

    // парсить headers (по waitAnswer() или available())
    bool beginParse() {
        _length = 0;
        _close = 0;
        bool conn = 0;
        bool eol = 0;
        while (client.connected()) {
            String line = client.readStringUntil('\n');
            if (!conn && line.startsWith(F("Connection: "))) conn = 1, _close = line.indexOf(F("close")) > 0;
            else if (!_length && line.startsWith(F("Content-Length: "))) _length = atol(line.c_str() + 16);
            else if (line == "\r") {
                eol = 1;
                break;
            }
            yield();
        }
        if (!eol || !_length) {  // !eol == disonnected
            flush();
            _length = 0;
            return 0;
        }
        return 1;
    }

    // размер пакета
    size_t length() {
        return _length;
    }

    // парсить пакет в буфер размера length()
    bool parse(char* buf) {
        bool ok = (client.readBytes(buf, _length) == _length);
        flush();
        return ok;
    }

    // клиент ждёт ответа
    bool isWaiting() {
        return client.connected() && _wait_f;
    }

    // остановить клиента
    void stop() {
        client.stop();
        _wait_f = 0;
    }

    // прочитать ответ, снять флаг ожидания, остановить если connection close
    void flush() {
        while (client.available()) {
            client.read();
            yield();
        }
        if (_close) stop();
        _wait_f = 0;
    }

    Client& client;

   private:
    const char* _host;
    uint16_t _port;
    bool _wait_f = 0;
    uint16_t _timeout;
    bool _close = 0;
    size_t _length = 0;
};