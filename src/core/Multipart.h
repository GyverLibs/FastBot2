#pragma once
#include <Arduino.h>
#include <GyverHTTP.h>
#include <StringUtils.h>

#include "../bot_config.h"
#include "core/api.h"

#ifdef FB_ESP_BUILD
#include <FS.h>
#endif

#ifndef FB_NO_FILE
#define FB_ATTACH "attach://"

namespace fb {

class Multipart : public StreamWriter {
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
    Multipart(const Text& name, Type type, ::File& file, bool edit) : StreamWriter(&file, file.size()), _name(name), _type(type), _edit(edit) {
        _init();
        setBlockSize(FB_BLOCK_SIZE);
    }
#endif
    Multipart(const Text& name, Type type, const uint8_t* bytes, size_t length, bool pgm, bool edit) : StreamWriter(bytes, length, pgm), _name(name), _type(type), _edit(edit) {
        _init();
        setBlockSize(FB_BLOCK_SIZE);
    }
    Multipart(const Text& name, Type type, const Text& urlid, bool edit) : _name(name), _type(type), _urlid(urlid), _edit(edit) {
        _init();
        setBlockSize(FB_BLOCK_SIZE);
    }

    const Text getAttachName() const {
        return _attachName;
    }

    Text getFormName() const {
        if (_edit) return _attachName + su::SL(FB_ATTACH);  // сместить указатель на random id
        else return getType();
    }

    const Text& getFileName() const {
        return _name;
    }

    const __FlashStringHelper* getType() const {
        switch (_type) {
            case Type::photo: return tg_api::photo;
            case Type::audio: return tg_api::audio;
            case Type::document: return tg_api::document;
            case Type::video: return tg_api::video;
            case Type::animation: return tg_api::animation;
            case Type::voice: return tg_api::voice;
            case Type::video_note: return tg_api::video_note;
        }
        return F("");
    }

    // return sendXXX or editMessageMedia
    const __FlashStringHelper* getCmd() const {
        if (_edit) return tg_cmd::editMessageMedia;

        switch (_type) {
            case Type::photo: return tg_cmd::sendPhoto;
            case Type::audio: return tg_cmd::sendAudio;
            case Type::document: return tg_cmd::sendDocument;
            case Type::video: return tg_cmd::sendVideo;
            case Type::animation: return tg_cmd::sendAnimation;
            case Type::voice: return tg_cmd::sendVoice;
            case Type::video_note: return tg_cmd::sendVideoNote;
            default: return F("");
        }
    }

    bool isFile() const {
        return !_urlid;
    }

    const Text& getUrlid() const {
        return _urlid;
    }

   protected:
    const Text _name;
    const Type _type;
    const Text _urlid;
    const bool _edit;
    char _attachName[su::SL(FB_ATTACH) + su::SL("ffffffff") + 1];

    void _init() {
        strcpy(_attachName, FB_ATTACH);
        ltoa(random(0xffffffff), _attachName + su::SL(FB_ATTACH), HEX);
    }
};

}  // namespace fb
#endif