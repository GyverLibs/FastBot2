#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "MessageRead.h"
#include "UserRead.h"
#include "../EntryAccess.h"
#include "../api.h"

namespace fb {

// https://core.telegram.org/bots/api#callbackquery
struct QueryRead : public EntryAccess {
    QueryRead(gson::Entry entry) : EntryAccess(entry) {}

    // callback id
    Text id() {
        return entry[tg_apih::id];
    }

    // callback data
    Text data() {
        return entry[tg_apih::data];
    }

    // отправитель коллбэка
    UserRead from() {
        return UserRead(entry[tg_apih::from]);
    }

    // сообщение
    MessageRead message() {
        return MessageRead(entry[tg_apih::message]);
    }
};

}  // namespace fb