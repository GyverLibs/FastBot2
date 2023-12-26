#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/keys.h"

namespace fb {

using sutil::AnyText;

class Chat {
   public:
    Chat(gson::Entry entry) : entry(entry) {}

    // id чата
    AnyText id() {
        return entry[fbhash::id];
    }

    // тип чата: private, group, supergroup, channel
    AnyText type() {
        return entry[fbhash::type];
    }

    // название чата (для supergroups, channels, group chats)
    AnyText title() {
        return entry[fbhash::title];
    }

    // имя чата (для private chats, supergroups, channels)
    AnyText username() {
        return entry[fbhash::username];
    }

    // имя (для private chat)
    AnyText first_name() {
        return entry[fbhash::first_name];
    }

    // фамилия (для private chat)
    AnyText last_name() {
        return entry[fbhash::last_name];
    }

    // в supergroup включены темы
    AnyText is_forum() {
        return entry[fbhash::is_forum];
    }

    gson::Entry entry;
};

}