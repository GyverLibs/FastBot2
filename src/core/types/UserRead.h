#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#user
struct UserRead {
    UserRead(gson::Entry entry) : entry(entry) {}

    // id юзера
    su::Text id() {
        return entry[fbhash::id];
    }

    // бот или нет
    su::Text isBot() {
        return entry[fbhash::is_bot];
    }

    // имя
    su::Text firstName() {
        return entry[fbhash::first_name];
    }

    // фамилия
    su::Text lastName() {
        return entry[fbhash::last_name];
    }

    // юзернейм
    su::Text username() {
        return entry[fbhash::username];
    }

    // код страны https://en.wikipedia.org/wiki/IETF_language_tag
    su::Text languageCode() {
        return entry[fbhash::language_code];
    }

    // true - премиум юзер
    su::Text isPremium() {
        return entry[fbhash::is_premium];
    }

    // доступ к пакету данных
    gson::Entry entry;
};

}  // namespace fb