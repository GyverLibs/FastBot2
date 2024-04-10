#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "MessageRead.h"
#include "UserRead.h"
#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#callbackquery
struct QueryRead : public gson::Entry {
    QueryRead(gson::Entry entry) : gson::Entry(entry) {}

    // callback id
    su::Text id() {
        return (*this)[fbh::api::id];
    }

    // callback data
    su::Text data() {
        return (*this)[fbh::api::data];
    }

    // отправитель коллбэка
    UserRead from() {
        return UserRead((*this)[fbh::api::from]);
    }

    // сообщение
    MessageRead message() {
        return MessageRead((*this)[fbh::api::message]);
    }
};

}  // namespace fb