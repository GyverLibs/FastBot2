#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/BracketAccess.h"
#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#chat
struct ChatRead : public BracketAccess {
    ChatRead(gson::Entry entry) : BracketAccess(entry) {}

    enum class Type : size_t {
        privateChat = su::SH("private"),
        group = su::SH("group"),
        supergroup = su::SH("supergroup"),
        channel = su::SH("channel"),
    };

    // id чата
    su::Text id() {
        return entry[fbh::api::id];
    }

    // тип чата: private_chat, group, supergroup, channel
    Type type() {
        return (Type)entry[fbh::api::type].hash();
    }

    // название чата (для supergroups, channels, group chats)
    su::Text title() {
        return entry[fbh::api::title];
    }

    // имя чата (для private chats, supergroups, channels)
    su::Text username() {
        return entry[fbh::api::username];
    }

    // имя (для private chat)
    su::Text firstName() {
        return entry[fbh::api::first_name];
    }

    // фамилия (для private chat)
    su::Text lastName() {
        return entry[fbh::api::last_name];
    }

    // описание чата
    su::Text description() {
        return entry[fbh::api::description];
    }

    // в supergroup включены темы
    su::Text isForum() {
        return entry[fbh::api::is_forum];
    }
};

}  // namespace fb