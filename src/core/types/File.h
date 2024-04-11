#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "Message.h"
#include "core/Multipart.h"

#ifndef FB_NO_FILE
namespace fb {

class File : protected Message {
    friend class ::VirtualFastBot2;

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
    File(const su::Text& name, Type type, ::File& file) : File(name, type, file, false) {}
#endif
    // отправить данные из byte буфера
    File(const su::Text& name, Type type, uint8_t* bytes, size_t length) : File(name, type, bytes, length, false) {}

    // отправить по ID файла в телеге или ссылкой (для document только GIF, PDF и ZIP)
    // https://core.telegram.org/bots/api#sending-files
    File(const su::Text& name, Type type, const su::Text& urlid) : File(name, type, urlid, false) {}

    using Message::chatID;
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
            if (caption.length()) p.addQS(api::caption, caption);
        } else {
            Message::makePacket(p);
            p[multipart.getType()] = multipart.getUrlid();
            if (caption.length()) p.addStringEsc(api::caption, caption);
        }
    }

#ifdef FS_H
    // отправить fs::File файл
    File(const su::Text& name, Type type, ::File& file, bool edit) : multipart(name, (Multipart::Type)type, file, edit) {}
#endif
    // отправить данные из byte буфера
    File(const su::Text& name, Type type, uint8_t* bytes, size_t length, bool edit) : multipart(name, (Multipart::Type)type, bytes, length, edit) {}

    // отправить по ID файла в телеге или ссылкой (для document только GIF, PDF и ZIP)
    File(const su::Text& name, Type type, const su::Text& urlid, bool edit) : multipart(name, (Multipart::Type)type, urlid, edit) {}
};

// https://core.telegram.org/bots/api#editmessagemedia
class FileEdit : protected File {
    friend class ::VirtualFastBot2;

   public:
#ifdef FS_H
    // отправить fs::File файл
    FileEdit(const su::Text& name, Type type, ::File& file) : File(name, type, file, true) {}
#endif
    FileEdit(const su::Text& name, Type type, uint8_t* bytes, size_t length) : File(name, type, bytes, length, true) {}

    // document by url - GIF, PDF and ZIP
    // https://core.telegram.org/bots/api#sending-files
    FileEdit(const su::Text& name, Type type, const su::Text& urlid) : File(name, type, urlid, true) {}

    // id сообщения
    uint32_t messageID;

    using File::caption;
    using File::chatID;
    using File::multipart;
    using Message::setInlineMenu;

   protected:
    void makePacket(Packet& p) const {
        if (multipart.isFile()) {
            File::Message::makeQS(p);
            p.addQS(api::message_id, messageID);
            {
                p.beginQS(api::media);
                p.beginObj();
                p[api::type] = multipart.getType();
                p[api::media] = multipart.getAttachName();
                if (caption.length()) p.addStringEsc(api::caption, caption);
                p.endObj();
                p.end();
            }
        } else {
            File::Message::makePacket(p);
            p[api::message_id] = messageID;
            {
                p.beginObj(api::media);
                p[api::type] = multipart.getType();
                p[api::media] = multipart.getUrlid();
                if (caption.length()) p.addStringEsc(api::caption, caption);
                p.endObj();
            }
        }
    }
};

}  // namespace fb
#endif