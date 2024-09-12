#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "Message.h"
#include "../Multipart.h"

#ifndef FB_NO_FILE
namespace fb {

class File : protected Message {
    friend class ::FastBot2Client;

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
    // отправить fs::File файл
    File(const Text& name, Type type, ::File& file) : File(name, type, file, false) {}
#endif

    // отправить данные из byte/progmem буфера
    File(const Text& name, Type type, const uint8_t* bytes, size_t length, bool pgm = false) : File(name, type, bytes, length, pgm, false) {}

    // отправить по ID файла в телеге или ссылкой (для document только GIF, PDF и ZIP)
    // https://core.telegram.org/bots/api#sending-files
    File(const Text& name, Type type, const Text& urlid) : File(name, type, urlid, false) {}

    using Message::chatID;
    using Message::json;
    using Message::mode;
    using Message::notification;
    using Message::protect;
    using Message::reply;
    using Message::setInlineMenu;
    using Message::threadID;

    // заголовок
    String caption;

    Multipart multipart;

   protected:
    void makePacket(Packet& p) const {
        if (multipart.isFile()) {
            Message::makeQS(p);
            if (caption.length()) p.addQS(tg_api::caption, caption);
        } else {
            Message::makePacket(p);
            p[multipart.getType()] = multipart.getUrlid();
            if (caption.length()) p.addStringEsc(tg_api::caption, caption);
        }
    }

#ifdef FS_H
    // отправить fs::File файл
    File(const Text& name, Type type, ::File& file, bool edit) : multipart(name, (Multipart::Type)type, file, edit) {}
#endif

    // отправить данные из byte буфера
    File(const Text& name, Type type, const uint8_t* bytes, size_t length, bool pgm, bool edit) : multipart(name, (Multipart::Type)type, bytes, length, pgm, edit) {}

    // отправить по ID файла в телеге или ссылкой (для document только GIF, PDF и ZIP)
    File(const Text& name, Type type, const Text& urlid, bool edit) : multipart(name, (Multipart::Type)type, urlid, edit) {}
};

// https://core.telegram.org/bots/api#editmessagemedia
class FileEdit : protected File {
    friend class ::FastBot2Client;

   public:
#ifdef FS_H
    // отправить fs::File файл
    FileEdit(const Text& name, Type type, ::File& file) : File(name, type, file, true) {}
#endif

    FileEdit(const Text& name, Type type, const uint8_t* bytes, size_t length, bool pgm = false) : File(name, type, bytes, length, pgm, true) {}

    // document by url - GIF, PDF and ZIP
    // https://core.telegram.org/bots/api#sending-files
    FileEdit(const Text& name, Type type, const Text& urlid) : File(name, type, urlid, true) {}

    // id сообщения
    uint32_t messageID;

    using File::caption;
    using File::chatID;
    using File::multipart;
    using Message::json;
    using Message::setInlineMenu;

   protected:
    void makePacket(Packet& p) const {
        if (multipart.isFile()) {
            File::Message::makeQS(p);
            p.addQS(tg_api::message_id, messageID);
            {
                p.beginQS(tg_api::media);
                p.beginObj();
                p[tg_api::type] = multipart.getType();
                p[tg_api::media] = multipart.getAttachName();
                if (caption.length()) p.addStringEsc(tg_api::caption, caption);
                p.endObj();
                p.end();
            }
        } else {
            File::Message::makePacket(p);
            p[tg_api::message_id] = messageID;
            {
                p.beginObj(tg_api::media);
                p[tg_api::type] = multipart.getType();
                p[tg_api::media] = multipart.getUrlid();
                if (caption.length()) p.addStringEsc(tg_api::caption, caption);
                p.endObj();
            }
        }
    }
};

}  // namespace fb
#endif