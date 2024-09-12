#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "../EntryAccess.h"
#include "../api.h"

namespace fb {

// https://core.telegram.org/bots/api#chat
struct ChatRead : public EntryAccess {
    ChatRead(gson::Entry entry) : EntryAccess(entry) {}

    enum class Type : size_t {
        privateChat = SH("private"),
        group = SH("group"),
        supergroup = SH("supergroup"),
        channel = SH("channel"),
    };

    // id чата
    Text id() {
        return entry[tg_apih::id];
    }

    // тип чата: private_chat, group, supergroup, channel
    Type type() {
        return (Type)entry[tg_apih::type].hash();
    }

    // название чата (для supergroups, channels, group chats)
    Text title() {
        return entry[tg_apih::title];
    }

    // имя чата (для private chats, supergroups, channels)
    Text username() {
        return entry[tg_apih::username];
    }

    // имя (для private chat)
    Text firstName() {
        return entry[tg_apih::first_name];
    }

    // фамилия (для private chat)
    Text lastName() {
        return entry[tg_apih::last_name];
    }

    // описание чата
    Text description() {
        return entry[tg_apih::description];
    }

    // в supergroup включены темы
    Text isForum() {
        return entry[tg_apih::is_forum];
    }
};

}  // namespace fb