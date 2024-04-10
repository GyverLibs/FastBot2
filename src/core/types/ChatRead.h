#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#chat
struct ChatRead : public gson::Entry {
    ChatRead(gson::Entry entry) : gson::Entry(entry) {}

    enum class Type : size_t {
        privateChat = su::SH("private"),
        group = su::SH("group"),
        supergroup = su::SH("supergroup"),
        channel = su::SH("channel"),
    };

    // id чата
    su::Text id() {
        return (*this)[fbh::api::id];
    }

    // тип чата: private_chat, group, supergroup, channel
    Type type() {
        return (Type)(*this)[fbh::api::type].hash();
    }

    // название чата (для supergroups, channels, group chats)
    su::Text title() {
        return (*this)[fbh::api::title];
    }

    // имя чата (для private chats, supergroups, channels)
    su::Text username() {
        return (*this)[fbh::api::username];
    }

    // имя (для private chat)
    su::Text firstName() {
        return (*this)[fbh::api::first_name];
    }

    // фамилия (для private chat)
    su::Text lastName() {
        return (*this)[fbh::api::last_name];
    }

    // описание чата
    su::Text description() {
        return (*this)[fbh::api::description];
    }

    // в supergroup включены темы
    su::Text isForum() {
        return (*this)[fbh::api::is_forum];
    }
};

}  // namespace fb