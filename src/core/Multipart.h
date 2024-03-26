#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "core/api.h"
#include "core/config.h"

#ifdef FB_ESP_BUILD
#include <FS.h>
#endif

#define FB_ATTACH "attach://"
#define FB_ATTACH_LEN 9

namespace fb {

class Multipart : public Printable {
   public:
    enum class Type : uint8_t {
        photo,
        audio,
        document,
        video,
        animation,
        voice,
        video_note,
    };

#ifdef FB_ESP_BUILD
    Multipart(const su::Text& name, Type type, ::File& file, bool edit) : _name(name), _type(type), _file(&file), _edit(edit) {
        _init();
    }
#endif
    Multipart(const su::Text& name, Type type, uint8_t* bytes, size_t length, bool edit) : _name(name), _type(type), _bytes(bytes), _length(length), _edit(edit) {
        _init();
    }
    Multipart(const su::Text& name, Type type, const su::Text& urlid, bool edit) : _name(name), _type(type), _urlid(urlid), _edit(edit) {
        _init();
    }

    uint32_t length() const {
        return _length;
    }

    const su::Text getAttachName() const {
        return _attachName;
    }

    su::Text getFormName() const {
        if (_edit) return _attachName + FB_ATTACH_LEN;
        else return getType();
    }

    const su::Text& getFileName() const {
        return _name;
    }

    const __FlashStringHelper* getType() const {
        switch (_type) {
            case Type::photo:
                return fbapi::photo();
            case Type::audio:
                return fbapi::audio();
            case Type::document:
                return fbapi::document();
            case Type::video:
                return fbapi::video();
            case Type::animation:
                return fbapi::animation();
            case Type::voice:
                return fbapi::voice();
            case Type::video_note:
                return fbapi::video_note();
        }
        return F("");
    }

    // return sendXXX or editMessageMedia
    const __FlashStringHelper* getCmd() const {
        if (_edit) return fbcmd::editMessageMedia();

        switch (_type) {
            case Type::photo:
                return fbcmd::sendPhoto();
            case Type::audio:
                return fbcmd::sendAudio();
            case Type::document:
                return fbcmd::sendDocument();
            case Type::video:
                return fbcmd::sendVideo();
            case Type::animation:
                return fbcmd::sendAnimation();
            case Type::voice:
                return fbcmd::sendVoice();
            case Type::video_note:
                return fbcmd::sendVideoNote();
        }
        return F("");
    }

    size_t printTo(Print& p) const {
        if (!isFile()) return 0;
        size_t printed = 0;
#ifdef FB_ESP_BUILD
        if (_file) {
            uint8_t buf[FB_BLOCK_SIZE];
            size_t length = _file->size();
            while (length) {
                size_t curlen = min((size_t)FB_BLOCK_SIZE, length);
                _file->read(buf, curlen);
                printed += p.write(buf, curlen);
                length -= curlen;
            }
            return printed;
        }
#endif
        if (_bytes) {
            size_t length = _length;
            const uint8_t* bytesp = _bytes;
            while (length) {
                size_t curlen = min((size_t)FB_BLOCK_SIZE, length);
                printed += p.write(bytesp, curlen);
                bytesp += curlen;
                length -= curlen;
            }
        }
        return printed;
    }

    bool isFile() const {
        return !_urlid.valid();
    }

    const su::Text& getUrlid() const {
        return _urlid;
    }

   protected:
    const su::Text _name;
    const Type _type;
    const su::Text _urlid;
#ifdef FB_ESP_BUILD
    ::File* _file = nullptr;
#endif
    const uint8_t* _bytes = nullptr;
    const size_t _length = 0;
    const bool _edit;
    char _attachName[20] = FB_ATTACH;

    void _init() {
        ultoa(random(), _attachName + FB_ATTACH_LEN, HEX);
    }
};

}  // namespace fb