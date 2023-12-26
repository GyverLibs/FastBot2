#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/keys.h"

namespace fb {

using sutil::AnyText;

class User {
   public:
    User(gson::Entry entry) : entry(entry) {}

    // id юзера
    AnyText id() {
        return entry[fbhash::id];
    }

    // бот или нет
    AnyText is_bot() {
        return entry[fbhash::is_bot];
    }

    // имя
    AnyText first_name() {
        return entry[fbhash::first_name];
    }

    // фамилия
    AnyText last_name() {
        return entry[fbhash::last_name];
    }

    // юзернейм
    AnyText username() {
        return entry[fbhash::username];
    }

    // код страны https://en.wikipedia.org/wiki/IETF_language_tag
    AnyText language_code() {
        return entry[fbhash::language_code];
    }

    // true - премиум юзер
    AnyText is_premium() {
        return entry[fbhash::is_premium];
    }

    gson::Entry entry;
};

}