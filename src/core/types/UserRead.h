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
        return entry[fbh::api::id];
    }

    // бот или нет
    su::Text isBot() {
        return entry[fbh::api::is_bot];
    }

    // имя
    su::Text firstName() {
        return entry[fbh::api::first_name];
    }

    // фамилия
    su::Text lastName() {
        return entry[fbh::api::last_name];
    }

    // юзернейм
    su::Text username() {
        return entry[fbh::api::username];
    }

    // код страны https://en.wikipedia.org/wiki/IETF_language_tag
    su::Text languageCode() {
        return entry[fbh::api::language_code];
    }

    // true - премиум юзер
    su::Text isPremium() {
        return entry[fbh::api::is_premium];
    }

    // доступ к пакету данных
    gson::Entry entry;
};

}  // namespace fb