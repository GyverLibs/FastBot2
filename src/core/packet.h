#pragma once
#include <Arduino.h>
#include <GSON.h>

#include "../bot_config.h"
#include "Multipart.h"
#include "core/types/ID.h"

#define FB_BOUNDARY "---------FAST_BOT2"

namespace fb {

class Packet : public gson::Str {
    enum class Type : uint8_t {
        Json,
        File,
        Raw,
    };

   public:
    Packet() : gson::Str(200) {}

#if !defined(FB_NO_FILE) && (defined(ESP8266) || defined(ESP32))
    // отправка файла через multipart/form-data
    Packet(const Multipart& multipart, const String& token) : gson::Str(200) {
        this->multipart = &multipart;
        _beginPost(multipart.getCmd(), token);

        if (multipart.isFile()) {
            _type = Type::File;
            concat('?');
        } else {
            _type = Type::Json;
            _beginJson();
        }
    }
#endif
    // команда с JSON пакетом
    Packet(const __FlashStringHelper* cmd, const String& token) : gson::Str(200) {
        _type = Type::Json;
        _beginPost(cmd, token);
        _beginJson();
    }

    // команда с внешним JSON пакетом
    Packet(const __FlashStringHelper* cmd, const String& token, const Text& json) : gson::Str(200) {
        _type = Type::Raw;
        _beginPost(cmd, token);
        _beginHeaders();

        if (json.length()) {
            _jsonHeaders();
            bool noBrackets = (json[0] != '{');
            concat(json.length() + noBrackets * 2);
            concat(F("\r\n\r\n"));
            if (noBrackets) concat('{');
            concat(json);
            if (noBrackets) concat('}');
        } else {
            concat(F("\r\n"));
        }
    }

    // запрос на скачивание файла вида /file/bot.../<path>
    Packet(const Text& path, const String& token) : gson::Str(200) {
        _type = Type::Raw;
        concat(F("GET /file/bot"));  // TODO https proxy?
        concat(token);
        concat('/');
        concat(path);
        concat(F(
            " HTTP/1.1\r\n"
            "Host: " TELEGRAM_HOST "\r\n\r\n"));
    }

    // query string
    void beginQS(const __FlashStringHelper* key) {
        if (_qs_first) _qs_first = 0;
        else concat('&');
        concat(key);
        concat('=');
    }

    void addQS(const __FlashStringHelper* key, const Text& value) {
        beginQS(key);
        concat(value);
    }
    void addQS(const __FlashStringHelper* key, int32_t value) {
        beginQS(key);
        concat(value);
    }

    // print
    void printTo(Print& p) {
        switch (_type) {
#if !defined(FB_NO_FILE) && (defined(ESP8266) || defined(ESP32))
            case Type::File: {
                Text formName = multipart->getFormName();
                Text fileName = multipart->getFileName();
                size_t len = multipart->length();

                len += SL("--" FB_BOUNDARY "\r\n");
                len += SL("Content-Disposition: form-data; name=\"") + formName.length() + SL("\"; filename=\"") + fileName.length() + SL("\"\r\n\r\n");
                len += SL("\r\n--" FB_BOUNDARY "--");

                concat(F(
                    " HTTP/1.1\r\n"
                    "Host: " TELEGRAM_HOST
                    "\r\n"
                    "Content-Type: multipart/form-data; boundary=" FB_BOUNDARY
                    "\r\n"
                    "Content-Length: "));
                concat(len);
                concat(F(
                    "\r\n\r\n"
                    "--" FB_BOUNDARY
                    "\r\n"
                    "Content-Disposition: form-data; name=\""));
                concat(formName);
                concat(F("\"; filename=\""));
                concat(fileName);
                concat(F(
                    "\"\r\n"
                    "\r\n"));

                // print
                p.print(*this);
                multipart->printTo(p);
                p.print(F("\r\n--" FB_BOUNDARY "--"));
            } break;
#endif

            case Type::Json: {
                (*this)('}');
                
                uint16_t len = length() - _header_pos;
                char* ptr = (char*)buf() + _header_pos - 5;
                do {
                    uint8_t mod = len % 10;
                    len /= 10;
                    *ptr-- = mod + '0';
                } while (len);

                p.print(*this);
            } break;

            case Type::Raw:
                p.print(*this);
                break;

            default:
                break;
        }
    }

#if !defined(FB_NO_FILE) && (defined(ESP8266) || defined(ESP32))
    const Multipart* multipart = nullptr;
#endif

   private:
    Type _type = Type::Json;
    uint16_t _header_pos = 0;
    bool _qs_first = 1;

    void _beginPost(const __FlashStringHelper* cmd, const String& token) {
        concat(F("POST https://" TELEGRAM_HOST "/bot"));
        concat(token);
        concat('/');
        concat(cmd);
    }
    void _beginHeaders() {
        concat(F(
            " HTTP/1.1\r\n"
            "Host: " TELEGRAM_HOST
            "\r\n"
            "Cache-Control: no-cache\r\n"));
    }
    void _jsonHeaders() {
        concat(F(
            "Content-Type: application/json\r\n"
            "Content-Length: "));
    }
    void _beginJson() {
        _beginHeaders();
        _jsonHeaders();
        concat(F("     \r\n\r\n"));
        _header_pos = length();
        (*this)('{');
    }
};

}  // namespace fb