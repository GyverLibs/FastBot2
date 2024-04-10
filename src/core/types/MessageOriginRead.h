#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "ChatRead.h"
#include "UserRead.h"
#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#messageorigin
struct MessageOriginRead : public gson::Entry {
    MessageOriginRead(gson::Entry entry) : gson::Entry(entry) {}

    enum class Type : size_t {
        user = su::SH("user"),
        hiddenUser = su::SH("hidden_user"),
        chat = su::SH("chat"),
        channel = su::SH("channel"),
    };

    // тип отправителя: user, hidden_user, chat, channel
    Type type() {
        return (Type)(*this)[fbh::api::type].hash();
    }

    // дата оригинального сообщения
    su::Text date() {
        return (*this)[fbh::api::date];
    }

    // отправитель type == user
    UserRead senderUser() {
        return UserRead((*this)[fbh::api::sender_user]);
    }

    // отправитель type == chat
    ChatRead senderChat() {
        return ChatRead((*this)[fbh::api::sender_chat]);
    }

    // отправитель type == channel
    ChatRead chat() {
        return ChatRead((*this)[fbh::api::chat]);
    }
};

}  // namespace fb