#pragma once
#include <Arduino.h>
#include <GSON.h>

#include "config.h"

namespace fb {

using gson::string;

class Packet : public gson::string {
   public:
    Packet(const __FlashStringHelper* cmd, const String& token) {
        s += F("POST /bot");
        s += token;
        s += '/';
        s += cmd;
        s += F(
            " HTTP/1.1"
            "\r\nHost: " TELEGRAM_HOST
            "\r\nContent-Type: application/json"
            "\r\nContent-Length:      "
            "\r\n\r\n");

        _header_pos = s.length();
        string::beginObj();
    }
    void end() {
        string::endObj();
        string::end();
        uint16_t len = s.length() - _header_pos;
        char* p = (char*)s.c_str() + _header_pos - 5;
        do {
            uint8_t mod = len % 10;
            len /= 10;
            *p-- = mod + '0';
        } while (len);
    }

   private:
    uint8_t _header_pos = 0;
};

}  // namespace fb