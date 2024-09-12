#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "../EntryAccess.h"
#include "../api.h"

namespace fb {

// https://core.telegram.org/bots/api#user
struct UserRead : public EntryAccess {
    UserRead(gson::Entry entry) : EntryAccess(entry) {}

    // id юзера
    Text id() {
        return entry[tg_apih::id];
    }

    // бот или нет
    Text isBot() {
        return entry[tg_apih::is_bot];
    }

    // имя
    Text firstName() {
        return entry[tg_apih::first_name];
    }

    // фамилия
    Text lastName() {
        return entry[tg_apih::last_name];
    }

    // юзернейм
    Text username() {
        return entry[tg_apih::username];
    }

    // код страны https://en.wikipedia.org/wiki/IETF_language_tag
    Text languageCode() {
        return entry[tg_apih::language_code];
    }

    // true - премиум юзер
    Text isPremium() {
        return entry[tg_apih::is_premium];
    }
};

}  // namespace fb