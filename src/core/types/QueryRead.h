#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "MessageRead.h"
#include "UserRead.h"
#include "core/BracketAccess.h"
#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#callbackquery
struct QueryRead : public BracketAccess {
    QueryRead(gson::Entry entry) : BracketAccess(entry) {}

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
};

}  // namespace fb