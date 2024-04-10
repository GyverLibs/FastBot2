#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#user
struct UserRead : public gson::Entry {
    UserRead(gson::Entry entry) : gson::Entry(entry) {}

    // id юзера
    su::Text id() {
        return (*this)[fbh::api::id];
    }

    // бот или нет
    su::Text isBot() {
        return (*this)[fbh::api::is_bot];
    }

    // имя
    su::Text firstName() {
        return (*this)[fbh::api::first_name];
    }

    // фамилия
    su::Text lastName() {
        return (*this)[fbh::api::last_name];
    }

    // юзернейм
    su::Text username() {
        return (*this)[fbh::api::username];
    }

    // код страны https://en.wikipedia.org/wiki/IETF_language_tag
    su::Text languageCode() {
        return (*this)[fbh::api::language_code];
    }

    // true - премиум юзер
    su::Text isPremium() {
        return (*this)[fbh::api::is_premium];
    }
};

}  // namespace fb