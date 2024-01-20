#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "FastBot2_class.h"
#include "core/api.h"

namespace fb {

using sutil::AnyText;

class Chat {
    friend class ::FastBot2;

   public:
    Chat(gson::Entry entry) : entry(entry) {}

    enum class Type : size_t {
        private_chat = sutil::SH("private"),
        group = sutil::SH("group"),
        supergroup = sutil::SH("supergroup"),
        channel = sutil::SH("channel"),
    };

    // id чата
    AnyText id() {
        return entry[fbhash::id];
    }

    // тип чата: private_chat, group, supergroup, channel
    Type type() {
        return (Type)entry[fbhash::type].hash();
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

    // доступ к пакету данных
    gson::Entry entry;

   private:
};

}  // namespace fb