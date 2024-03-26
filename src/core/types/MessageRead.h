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
        return entry[fbh::text];
    }

    // id сообщения в этом чате
    su::Text id() {
        return entry[fbh::message_id];
    }

    // id темы в группе
    su::Text threadID() {
        return entry[fbh::message_thread_id];
    }

    // сообщение отправлено в топик форума
    su::Text isTopic() {
        return entry[fbh::is_topic_message];
    }

    // дата отправки или пересылки сообщения
    su::Text date() {
        return entry[fbh::date];
    }

    // дата изменения сообщения
    su::Text editDate() {
        return entry[fbh::edit_date];
    }

    // ================ SENDER ================

    // отправитель сообщения
    UserRead from() {
        return UserRead(entry[fbh::from]);
    }

    // бот, через которого пришло это сообщение
    UserRead viaBot() {
        return UserRead(entry[fbh::via_bot]);
    }

    // чат, которому принадлежит это сообщение
    ChatRead chat() {
        return ChatRead(entry[fbh::chat]);
    }

    // чат, если сообщение отправлено от имени чата
    ChatRead senderChat() {
        return ChatRead(entry[fbh::sender_chat]);
    }

    // ================ REPLY ================

    // сообщение является ответом на сообщение
    bool isReply() {
        return entry.includes(fbh::reply_to_message);
    }

    // сообщение, на которое отвечает это сообщение
    MessageRead reply() {
        return MessageRead(entry[fbh::reply_to_message]);
    }

    // ================ FORWARD ================

    // сообщение переслано из другого чата
    bool isForward() {
        return entry.includes(fbh::forward_origin);
    }

    // данные о пересланном сообщении
    MessageOriginRead forward() {
        return MessageOriginRead(entry[fbh::forward_origin]);
    }

    // ================ LOCATION ================

    // сообщение содержит геолокацию
    bool hasLocation() {
        return entry.includes(fbh::location);
    }

    // геолокация
    LocationRead location() {
        return LocationRead(entry[fbh::location]);
    }

    // ================ DOCUMENT ================

    // сообщение содержит документ
    bool hasDocument() {
        return entry.includes(fbh::document);
    }

    // документ
    DocumentRead document() {
        return DocumentRead(entry[fbh::document]);
    }

    // доступ к пакету данных
    gson::Entry entry;
};

}  // namespace fb