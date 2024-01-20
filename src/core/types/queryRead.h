#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "FastBot2_class.h"
#include "core/api.h"
#include "messageRead.h"
#include "user.h"

namespace fb {

using sutil::AnyText;

class QueryRead {
    friend class ::FastBot2;

   public:
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