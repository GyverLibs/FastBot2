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
struct MessageRead : public gson::Entry {
    MessageRead(gson::Entry entry) : gson::Entry(entry) {}

    // ================ INFO ================

    // текст сообщения
    su::Text text() {
        return (*this)[fbh::api::text];
    }

    // id сообщения в этом чате
    su::Text id() {
        return (*this)[fbh::api::message_id];
    }

    // id темы в группе
    su::Text threadID() {
        return (*this)[fbh::api::message_thread_id];
    }

    // сообщение отправлено в топик форума
    su::Text isTopic() {
        return (*this)[fbh::api::is_topic_message];
    }

    // дата отправки или пересылки сообщения
    su::Text date() {
        return (*this)[fbh::api::date];
    }

    // дата изменения сообщения
    su::Text editDate() {
        return (*this)[fbh::api::edit_date];
    }

    // ================ SENDER ================

    // отправитель сообщения
    UserRead from() {
        return UserRead((*this)[fbh::api::from]);
    }

    // бот, через которого пришло это сообщение
    UserRead viaBot() {
        return UserRead((*this)[fbh::api::via_bot]);
    }

    // чат, которому принадлежит это сообщение
    ChatRead chat() {
        return ChatRead((*this)[fbh::api::chat]);
    }

    // чат, если сообщение отправлено от имени чата
    ChatRead senderChat() {
        return ChatRead((*this)[fbh::api::sender_chat]);
    }

    // ================ REPLY ================

    // сообщение является ответом на сообщение
    bool isReply() {
        return (*this).includes(fbh::api::reply_to_message);
    }

    // сообщение, на которое отвечает это сообщение
    MessageRead reply() {
        return MessageRead((*this)[fbh::api::reply_to_message]);
    }

    // ================ FORWARD ================

    // сообщение переслано из другого чата
    bool isForward() {
        return (*this).includes(fbh::api::forward_origin);
    }

    // данные о пересланном сообщении
    MessageOriginRead forward() {
        return MessageOriginRead((*this)[fbh::api::forward_origin]);
    }

    // ================ LOCATION ================

    // сообщение содержит геолокацию
    bool hasLocation() {
        return (*this).includes(fbh::api::location);
    }

    // геолокация
    LocationRead location() {
        return LocationRead((*this)[fbh::api::location]);
    }

    // ================ DOCUMENT ================

    // сообщение содержит документ
    bool hasDocument() {
        return (*this).includes(fbh::api::document);
    }

    // документ
    DocumentRead document() {
        return DocumentRead((*this)[fbh::api::document]);
    }
};

}  // namespace fb