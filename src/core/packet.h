#pragma once
#include <Arduino.h>
#include <GSON.h>

#include "Multipart.h"
#include "../bot_config.h"

#define FB_BOUNDARY "---------FAST_BOT2"

namespace fb {

class Packet : public gson::string {
    enum class Type : uint8_t {
        Json,
        File,
        Raw,
    };

   public:
    Packet() : gson::string(200) {}

#ifndef FB_NO_FILE
    // отправка файла через multipart/form-data
    Packet(const Multipart& multipart, const String& token) : gson::string(200) {
        this->multipart = &multipart;
        _beginPost(multipart.getCmd(), token);

        if (multipart.isFile()) {
            _type = Type::File;
            s += '?';
        } else {
            _type = Type::Json;
            _beginJson();
        }
    }
#endif
    // команда с JSON пакетом
    Packet(const __FlashStringHelper* cmd, const String& token) : gson::string(200) {
        _type = Type::Json;
        _beginPost(cmd, token);
        _beginJson();
    }

    // команда с внешним JSON пакетом
    Packet(const __FlashStringHelper* cmd, const String& token, const String& json) : gson::string(200) {
        _type = Type::Raw;
        _beginPost(cmd, token);
        _beginHeaders();

        if (json.length()) {
            _jsonHeaders();
            bool noBrackets = (json.charAt(0) != '{');
            s += json.length() + noBrackets * 2;
            s += F("\r\n\r\n");
            if (noBrackets) s += '{';
            s += json;
            if (noBrackets) s += '}';
        } else {
            s += F("\r\n");
        }
    }

    // запрос на скачивание файла вида /file/bot.../<path>
    Packet(const Text& path, const String& token) : gson::string(200) {
        _type = Type::Raw;
        s += F("GET /file/bot");  // TODO https proxy?
        s += token;
        s += '/';
        path.addString(s);
        s += F(
            " HTTP/1.1\r\n"
            "Host: " TELEGRAM_HOST "\r\n\r\n");
    }

    // query string
    void beginQS(const __FlashStringHelper* key) {
        if (_qs_first) _qs_first = 0;
        else s += '&';
        addTextRaw(key);
        s += '=';
    }

    void addQS(const __FlashStringHelper* key, const Value& value) {
        beginQS(key);
        addTextRaw(value);
    }

    // print
    void printTo(Print& p) {
        switch (_type) {
#ifndef FB_NO_FILE
            case Type::File: {
                Text formName = multipart->getFormName();
                Text fileName = multipart->getFileName();
                size_t len = multipart->length();

                len += su::SL("--" FB_BOUNDARY "\r\n");
                len += su::SL("Content-Disposition: form-data; name=\"") + formName.length() + su::SL("\"; filename=\"") + fileName.length() + su::SL("\"\r\n\r\n");
                len += su::SL("\r\n--" FB_BOUNDARY "--");

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
                p.print(s);
                multipart->printTo(p);
                p.print(F("\r\n--" FB_BOUNDARY "--"));
            } break;
#endif

            case Type::Json: {
                gson::string::endObj();
                gson::string::end();
                uint16_t len = s.length() - _header_pos;
                char* ptr = (char*)s.c_str() + _header_pos - 5;
                do {
                    uint8_t mod = len % 10;
                    len /= 10;
                    *ptr-- = mod + '0';
                } while (len);

                p.print(s);
            } break;

            case Type::Raw:
                p.print(s);
                break;

            default:
                break;
        }
    }

#ifndef FB_NO_FILE
    const Multipart* multipart = nullptr;
#endif

   private:
    Type _type = Type::Json;
    uint16_t _header_pos = 0;
    bool _qs_first = 1;

    void _beginPost(const __FlashStringHelper* cmd, const String& token) {
        escapeDefault(false);
        s += F("POST https://" TELEGRAM_HOST "/bot");
        s += token;
        s += '/';
        s += cmd;
    }
    void _beginHeaders() {
        s += F(
            " HTTP/1.1\r\n"
            "Host: " TELEGRAM_HOST
            "\r\n"
            "Cache-Control: no-cache\r\n");
    }
    void _jsonHeaders() {
        s += F(
            "Content-Type: application/json\r\n"
            "Content-Length: ");
    }
    void _beginJson() {
        _beginHeaders();
        _jsonHeaders();
        s += F("     \r\n\r\n");
        _header_pos = s.length();
        gson::string::beginObj();
    }
};

}  // namespace fb