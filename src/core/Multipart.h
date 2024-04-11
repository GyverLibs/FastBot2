#pragma once
#include <Arduino.h>
#include <StringUtils.h>

#include "core/api.h"
#include "core/config.h"

#ifdef FB_ESP_BUILD
#include <FS.h>
#endif

#ifndef FB_NO_FILE
#define FB_ATTACH "attach://"

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

#ifdef FS_H
    Multipart(const su::Text& name, Type type, ::File& file, bool edit) : _name(name), _type(type), _file(&file), _length(file.size()), _edit(edit) {
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
        if (_edit) return _attachName + su::SL(FB_ATTACH);
        else return getType();
    }

    const su::Text& getFileName() const {
        return _name;
    }

    const __FlashStringHelper* getType() const {
        switch (_type) {
            case Type::photo:
                return api::photo;
            case Type::audio:
                return api::audio;
            case Type::document:
                return api::document;
            case Type::video:
                return api::video;
            case Type::animation:
                return api::animation;
            case Type::voice:
                return api::voice;
            case Type::video_note:
                return api::video_note;
        }
        return F("");
    }

    // return sendXXX or editMessageMedia
    const __FlashStringHelper* getCmd() const {
        if (_edit) return cmd::editMessageMedia;

        switch (_type) {
            case Type::photo:
                return cmd::sendPhoto;
            case Type::audio:
                return cmd::sendAudio;
            case Type::document:
                return cmd::sendDocument;
            case Type::video:
                return cmd::sendVideo;
            case Type::animation:
                return cmd::sendAnimation;
            case Type::voice:
                return cmd::sendVoice;
            case Type::video_note:
                return cmd::sendVideoNote;
            default:
                return F("");
        }
    }

    size_t printTo(Print& p) const {
        if (!isFile()) return 0;
        size_t printed = 0;
        size_t length = _length;
#ifdef FS_H
        if (_file) {
            uint8_t* buf = new uint8_t[FB_BLOCK_SIZE];
            if (!buf) return 0;
            while (length) {
                size_t curlen = min((size_t)FB_BLOCK_SIZE, length);
                size_t read = _file->read(buf, curlen);
                printed += p.write(buf, read);
                length -= curlen;
            }
            delete[] buf;
            return printed;
        }
#endif
        if (_bytes) {
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
#ifdef FS_H
    ::File* _file = nullptr;
#endif
    const uint8_t* _bytes = nullptr;
    const size_t _length = 0;
    const bool _edit;
    char _attachName[su::SL(FB_ATTACH) + su::SL("ffffffff") + 1];

    void _init() {
        strcpy(_attachName, FB_ATTACH);
        ltoa(random(0xffffffff), _attachName + su::SL(FB_ATTACH), HEX);
    }
};

}  // namespace fb
#endif