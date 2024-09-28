#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "../EntryAccess.h"
#include "../api.h"
#include "ChatRead.h"
#include "DocumentRead.h"
#include "LocationRead.h"
#include "MessageOriginRead.h"
#include "UserRead.h"

namespace fb {

// https://core.telegram.org/bots/api#message
struct MessageRead : public EntryAccess {
    MessageRead(EntryAccess entry) : EntryAccess(entry) {}

    // ================ INFO ================

    // текст сообщения
    Text text() {
        return entry[tg_apih::text];
    }

    // подпись файла
    Text caption() {
        return entry[tg_apih::caption];
    }

    // id сообщения в этом чате
    Text id() {
        return entry[tg_apih::message_id];
    }

    // id темы в группе
    Text threadID() {
        return entry[tg_apih::message_thread_id];
    }

    // сообщение отправлено в топик форума
    Text isTopic() {
        return entry[tg_apih::is_topic_message];
    }

    // дата отправки или пересылки сообщения
    Text date() {
        return entry[tg_apih::date];
    }

    // дата изменения сообщения
    Text editDate() {
        return entry[tg_apih::edit_date];
    }

    // ================ SENDER ================

    // отправитель сообщения
    UserRead from() {
        return UserRead(entry[tg_apih::from]);
    }

    // бот, через которого пришло это сообщение
    UserRead viaBot() {
        return UserRead(entry[tg_apih::via_bot]);
    }

    // чат, которому принадлежит это сообщение
    ChatRead chat() {
        return ChatRead(entry[tg_apih::chat]);
    }

    // чат, если сообщение отправлено от имени чата
    ChatRead senderChat() {
        return ChatRead(entry[tg_apih::sender_chat]);
    }

    // ================ REPLY ================

    // сообщение является ответом на сообщение
    bool isReply() {
        return entry.has(tg_apih::reply_to_message);
    }

    // сообщение, на которое отвечает это сообщение
    MessageRead reply() {
        return MessageRead(entry[tg_apih::reply_to_message]);
    }

    // ================ FORWARD ================

    // сообщение переслано из другого чата
    bool isForward() {
        return entry.has(tg_apih::forward_origin);
    }

    // данные о пересланном сообщении
    MessageOriginRead forward() {
        return MessageOriginRead(entry[tg_apih::forward_origin]);
    }

    // ================ LOCATION ================

    // сообщение содержит геолокацию
    bool hasLocation() {
        return entry.has(tg_apih::location);
    }

    // геолокация
    LocationRead location() {
        return LocationRead(entry[tg_apih::location]);
    }

    // ================ DOCUMENT ================

    // сообщение содержит документ
    bool hasDocument() {
        return entry.has(tg_apih::document);
    }

    // документ
    DocumentRead document() {
        return DocumentRead(entry[tg_apih::document]);
    }
};

}  // namespace fb