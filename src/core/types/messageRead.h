#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "ChatRead.h"
#include "DocumentRead.h"
#include "LocationRead.h"
#include "MessageOriginRead.h"
#include "UserRead.h"
#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#message
struct MessageRead {
    MessageRead(gson::Entry entry) : entry(entry) {}

    // ================ INFO ================

    // текст сообщения
    su::Text text() {
        return entry[fbhash::text];
    }

    // id сообщения в этом чате
    su::Text id() {
        return entry[fbhash::message_id];
    }

    // id темы в группе
    su::Text threadID() {
        return entry[fbhash::message_thread_id];
    }

    // сообщение отправлено в топик форума
    su::Text isTopic() {
        return entry[fbhash::is_topic_message];
    }

    // дата отправки или пересылки сообщения
    su::Text date() {
        return entry[fbhash::date];
    }

    // дата изменения сообщения
    su::Text editDate() {
        return entry[fbhash::edit_date];
    }

    // ================ SENDER ================

    // отправитель сообщения
    UserRead from() {
        return UserRead(entry[fbhash::from]);
    }

    // бот, через которого пришло это сообщение
    UserRead viaBot() {
        return UserRead(entry[fbhash::via_bot]);
    }

    // чат, которому принадлежит это сообщение
    ChatRead chat() {
        return ChatRead(entry[fbhash::chat]);
    }

    // чат, если сообщение отправлено от имени чата
    ChatRead senderChat() {
        return ChatRead(entry[fbhash::sender_chat]);
    }

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
    MessageOriginRead forward() {
        return MessageOriginRead(entry[fbhash::forward_origin]);
    }

    // ================ LOCATION ================

    // сообщение содержит геолокацию
    bool hasLocation() {
        return entry.includes(fbhash::location);
    }

    // геолокация
    LocationRead location() {
        return LocationRead(entry[fbhash::location]);
    }

    // ================ DOCUMENT ================

    // сообщение содержит документ
    bool hasDocument() {
        return entry.includes(fbhash::document);
    }

    // документ
    DocumentRead document() {
        return DocumentRead(entry[fbhash::document]);
    }

    // доступ к пакету данных
    gson::Entry entry;
};

}  // namespace fb