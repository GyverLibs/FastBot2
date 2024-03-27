#pragma once
#include <Arduino.h>
#include <GSON.h>

#include "Multipart.h"
#include "config.h"

#define FB_BOUNDARY "---------FAST_BOT2"
#define FB_BOUNDARY_LEN 18

namespace fb {

class Packet : public gson::string {
   public:
    // constructor
    Packet(const Multipart& multipart, const String& token) : multipart(&multipart) {
        _begin(multipart.getCmd(), token);
        if (multipart.isFile()) s += '?';
        else _beginJson();
    }

    Packet(const __FlashStringHelper* cmd, const String& token) {
        _begin(cmd, token);
        _beginJson();
    }

    // query string
    void beginQS(const __FlashStringHelper* key) {
        if (_qs_first) _qs_first = 0;
        else s += '&';
        addText(key);
        s += '=';
    }

    void addQS(const __FlashStringHelper* key, const su::Value& value) {
        beginQS(key);
        addText(value);
    }

    // print
    void printTo(Print* p) {
        if (multipart && multipart->isFile()) {
            su::Text formName = multipart->getFormName();
            su::Text fileName = multipart->getFileName();
            uint32_t len = multipart->length();
            len += 2 + FB_BOUNDARY_LEN + 2;                              // --FB_BOUNDARYrn
            len += 38 + formName.length() + 13 + fileName.length() + 5;  // Content-Disposition: form-data; name=""; filename=""rnrn
            len += 2 + 2 + FB_BOUNDARY_LEN + 2;                          // rn--FB_BOUNDARY--

            s += F(
                " HTTP/1.1\r\n"
                "Host: " TELEGRAM_HOST
                "\r\n"
                "Content-Type: multipart/form-data; boundary=" FB_BOUNDARY
                "\r\n"
                "Content-Length: ");
            s += len;
            s += F(
                "\r\n\r\n"
                "--" FB_BOUNDARY
                "\r\n"
                "Content-Disposition: form-data; name=\"");
            formName.addString(s);
            s += F("\"; filename=\"");
            fileName.addString(s);
            s += F(
                "\"\r\n"
                "\r\n");

            // print
            p->print(s);
            p->print(*multipart);
            p->print(F("\r\n--" FB_BOUNDARY "--"));

        } else {
            gson::string::endObj();
            gson::string::end();
            uint16_t len = s.length() - _header_pos;
            char* ptr = (char*)s.c_str() + _header_pos - 5;
            do {
                uint8_t mod = len % 10;
                len /= 10;
                *ptr-- = mod + '0';
            } while (len);

            // print
            p->print(s);
        }
    }

    const Multipart* multipart = nullptr;

   private:
    uint16_t _header_pos = 0;
    bool _qs_first = 1;

    void _begin(const __FlashStringHelper* cmd, const String& token) {
        s += F("POST /bot");
        s += token;
        s += '/';
        s += cmd;
    }
    void _beginJson() {
        s += F(
            " HTTP/1.1\r\n"
            "Host: " TELEGRAM_HOST
            "\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length:      \r\n"
            "\r\n");

        _header_pos = s.length();
        gson::string::beginObj();
    }
};

}  // namespace fb