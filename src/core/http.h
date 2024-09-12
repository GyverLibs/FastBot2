#pragma once
#include <Arduino.h>
#include <GyverHTTP.h>

#include "../bot_config.h"

namespace fb {

class Http {
   public:
    Http(Client& client) : http(client, TELEGRAM_HOST, TELEGRAM_PORT) {}

    // установить proxy
    void setProxy(const char* host, uint16_t port) {
        http.setHost(host, port);
        _ip.fromString(host);
        _port = port;
    }

    // установить proxy
    void setProxy(const IPAddress& ip, uint16_t port) {
        http.setHost(ip, port);
        _ip = ip;
        _port = port;
    }

    // получить proxy IP
    const IPAddress& getProxyIP() {
        return _ip;
    }

    // Получить proxy port
    uint16_t getProxyPort() {
        return _port;
    }

    // удалить proxy
    void clearProxy() {
        http.setHost(TELEGRAM_HOST, TELEGRAM_PORT);
    }

   protected:
    ghttp::Client http;

   private:
    IPAddress _ip;
    uint16_t  _port;
};

}  // namespace fb