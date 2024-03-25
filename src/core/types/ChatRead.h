#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#chat
struct ChatRead {
    ChatRead(gson::Entry entry) : entry(entry) {}

    enum class Type : size_t {
        privateChat = su::SH("private"),
        group = su::SH("group"),
        supergroup = su::SH("supergroup"),
        channel = su::SH("channel"),
    };

    // id чата
    su::Text id() {
        return entry[fbh::id];
    }

    // тип чата: private_chat, group, supergroup, channel
    Type type() {
        return (Type)entry[fbh::type].hash();
    }

    // название чата (для supergroups, channels, group chats)
    su::Text title() {
        return entry[fbh::title];
    }

    // имя чата (для private chats, supergroups, channels)
    su::Text username() {
        return entry[fbh::username];
    }

    // имя (для private chat)
    su::Text firstName() {
        return entry[fbh::first_name];
    }

    // фамилия (для private chat)
    su::Text lastName() {
        return entry[fbh::last_name];
    }

    // описание чата
    su::Text description() {
        return entry[fbh::description];
    }

    // в supergroup включены темы
    su::Text isForum() {
        return entry[fbh::is_forum];
    }

    // доступ к пакету данных
    gson::Entry entry;
};

}  // namespace fb