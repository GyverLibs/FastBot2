#pragma once
#include <Arduino.h>
#include <GSON.h>

#include "Multipart.h"
#include "config.h"

#define FB_BOUNDARY "---------FAST_BOT2"

namespace fb {

class Packet : public gson::string {
    enum class Type : uint8_t {
        Json,
        File,
        Simple,
    };

   public:
    Packet() : gson::string(200) {}

#ifndef FB_NO_FILE
    // отправка файла через multipart/form-data
    size_t beginMultipart(const Multipart& multipart, const String& token) {
        this->multipart = &multipart;
        _begin(multipart.getCmd(), token);

        if (multipart.isFile()) {
            _type = Type::File;
            s += '?';
        } else {
            _type = Type::Json;
            _beginJson();
        }
        return su::hash_P((PGM_P)multipart.getCmd());
    }
#endif

    // команда с JSON пакетом
    size_t beginJson(const __FlashStringHelper* cmd, const String& token) {
        _type = Type::Json;
        _begin(cmd, token);
        _beginJson();
        return su::hash_P((PGM_P)cmd);
    }

    // запрос на скачивание файла вида /file/bot.../<path>
    size_t beginDownload(const su::Text& path, const String& token) {
        _type = Type::Simple;
        s += F("GET /file/bot");
        s += token;
        s += '/';
        path.addString(s);
        s += F(
            " HTTP/1.1\r\n"
            "Host: " TELEGRAM_HOST "\r\n\r\n");
        return 0;
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
    void printTo(Print& p) {
        switch (_type) {
#ifndef FB_NO_FILE
            case Type::File: {
                su::Text formName = multipart->getFormName();
                su::Text fileName = multipart->getFileName();
                uint32_t len = multipart->length();

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
                p.print(*multipart);
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

                // print
                p.print(s);
            } break;

            case Type::Simple: {
                p.print(s);
            } break;

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

    void _begin(const __FlashStringHelper* cmd, const String& token) {
        escapeDefault(false);
        s += F("GET /bot");
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