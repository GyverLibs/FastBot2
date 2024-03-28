#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "MessageRead.h"
#include "UserRead.h"
#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#callbackquery
struct QueryRead {
    QueryRead(gson::Entry entry) : entry(entry) {}

    // callback id
    su::Text id() {
        return entry[fbh::api::id];
    }

    // callback data
    su::Text data() {
        return entry[fbh::api::data];
    }

    // отправитель коллбэка
    UserRead from() {
        return UserRead(entry[fbh::api::from]);
    }

    // сообщение
    MessageRead message() {
        return MessageRead(entry[fbh::api::message]);
    }

    // доступ к пакету данных
    gson::Entry entry;
};

}  // namespace fb