#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/api.h"
#include "messageRead.h"
#include "user.h"

namespace fb {

using sutil::AnyText;

struct QueryRead {
    QueryRead(gson::Entry entry) : entry(entry) {}

    // callback id
    AnyText id() {
        return entry[fbhash::id];
    }

    // callback data
    AnyText data() {
        return entry[fbhash::data];
    }

    // отправитель коллбэка
    User from() {
        return User(entry[fbhash::from]);
    }

    // сообщение
    MessageRead message() {
        return MessageRead(entry[fbhash::message]);
    }

    // доступ к пакету данных
    gson::Entry entry;
};

}  // namespace fb