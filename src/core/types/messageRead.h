#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "FastBot2_class.h"
#include "chat.h"
#include "core/api.h"
#include "locationRead.h"
#include "user.h"

namespace fb {

using sutil::AnyText;

class MessageOrigin {
    friend class ::FastBot2;

   public:
    MessageOrigin(gson::Entry entry) : entry(entry) {}

    enum class Type : size_t {
        user = sutil::SH("user"),
        hidden_user = sutil::SH("hidden_user"),
        chat = sutil::SH("chat"),
        channel = sutil::SH("channel"),
    };

    // тип отправителя: user, hidden_user, chat, channel
    Type type() {
        return (Type)entry[fbhash::type].hash();
    }

    // дата оригинального сообщения
    AnyText date() {
        return entry[fbhash::date];
    }

    // отправитель type == user
    User sender_user() {
        return User(entry[fbhash::sender_user]);
    }

    // отправитель type == chat
    Chat sender_chat() {
        return Chat(entry[fbhash::sender_chat]);
    }

    // отправитель type == channel
    Chat chat() {
        return Chat(entry[fbhash::chat]);
    }

    // доступ к пакету данных
    gson::Entry entry;
};

class MessageRead {
    friend class ::FastBot2;

   public:
    MessageRead(gson::Entry entry) : entry(entry) {}

    // ================ REPLY ================

    // сообщение является ответом на сообщение
    bool isReply() {
        return entry.includes(fbhash::reply_to_message);
    }

    // сообщение, на которое отвечает это сообщение
    MessageRead reply() {
        return MessageRead(entry[fbhash::reply_to_message]);
    }

    // ================ FORWARD ================

    // сообщение переслано из другого чата
    bool isForward() {
        return entry.includes(fbhash::forward_origin);
    }

    // данные о пересланном сообщении
    MessageOrigin forward() {
        return MessageOrigin(entry[fbhash::forward_origin]);
    }

    // ================ MISC ================

    // текст сообщения
    AnyText text() {
        return entry[fbhash::text];
    }

    // id сообщения в этом чате
    AnyText message_id() {
        return entry[fbhash::message_id];
    }

    // id темы в группе
    AnyText thread_id() {
        return entry[fbhash::message_thread_id];
    }

    // сообщение отправлено в топик форума
    AnyText is_topic() {
        return entry[fbhash::is_topic_message];
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
        return entry.includes(fbhash::location);
    }

    // геолокация
    LocationRead location() {
        return LocationRead(entry[fbhash::location]);
    }

    // доступ к пакету данных
    gson::Entry entry;
};

}  // namespace fb