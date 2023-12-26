#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "chat.h"
#include "core/keys.h"
#include "locationRead.h"
#include "user.h"

namespace fb {

using sutil::AnyText;

class MessageRead {
   public:
    MessageRead(gson::Entry entry) : entry(entry) {}

    // ================ REPLY ================

    // сообщение является ответом на сообщение
    bool isReply() {
        return entry.includes(fbhash::reply_to_message);
    }

    // сообщение, на которое отвечает это сообщение
    MessageRead reply_to_message() {
        return MessageRead(entry[fbhash::reply_to_message]);
    }

    // ================ FORWARD ================

    // сообщение переслано из другого чата
    bool isForward() {
        return entry.includes(fbhash::forward_from) || entry.includes(fbhash::forward_from_chat);
    }

    // дата отправки оригинального пересланного сообщения
    AnyText forward_date() {
        return entry[fbhash::forward_date];
    }

    // отправитель оригинального пересланного сообщения
    User forward_from() {
        return User(entry[fbhash::forward_from]);
    }

    // чат, из которого переслали это сообщение
    Chat forward_from_chat() {
        return Chat(entry[fbhash::forward_from_chat]);
    }

    // id сообщения в канале, если оно переслано из канала
    AnyText forward_from_message_id() {
        return entry[fbhash::forward_from_message_id];
    }

    // ================ MISC ================

    // текст сообщения
    AnyText text() {
        return entry[fbhash::text];
    }

    // id сообщения в этом чате
    AnyText id() {
        return entry[fbhash::message_id];
    }

    // id темы в группе
    AnyText thread_id() {
        return entry[fbhash::message_thread_id];
    }

    // дата отправки или пересылки сообщения
    AnyText date() {
        return entry[fbhash::date];
    }

    // дата изменения сообщения
    AnyText edit_date() {
        return entry[fbhash::edit_date];
    }

    // отправитель сообщения
    User from() {
        return User(entry[fbhash::from]);
    }

    // бот, через которого пришло это сообщение
    User via_bot() {
        return User(entry[fbhash::via_bot]);
    }

    // чат, которому принадлежит это сообщение
    Chat chat() {
        return Chat(entry[fbhash::chat]);
    }

    // чат, если сообщение отправлено от имени чата
    Chat sender_chat() {
        return Chat(entry[fbhash::sender_chat]);
    }

    // сообщение содержит геолокацию
    bool hasLocation() {
        return entry[fbhash::location].valid();
    }

    // геолокация
    LocationRead location() {
        return LocationRead(entry[fbhash::location]);
    }

    gson::Entry entry;
};

}  // namespace fb