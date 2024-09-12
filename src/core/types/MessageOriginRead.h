#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "ChatRead.h"
#include "UserRead.h"
#include "../EntryAccess.h"
#include "../api.h"

namespace fb {

// https://core.telegram.org/bots/api#messageorigin
struct MessageOriginRead : public EntryAccess {
    MessageOriginRead(gson::Entry entry) : EntryAccess(entry) {}

    enum class Type : size_t {
        user = SH("user"),
        hiddenUser = SH("hidden_user"),
        chat = SH("chat"),
        channel = SH("channel"),
    };

    // тип отправителя: user, hidden_user, chat, channel
    Type type() {
        return (Type)entry[tg_apih::type].hash();
    }

    // дата оригинального сообщения
    Text date() {
        return entry[tg_apih::date];
    }

    // отправитель type == user
    UserRead senderUser() {
        return UserRead(entry[tg_apih::sender_user]);
    }

    // отправитель type == chat
    ChatRead senderChat() {
        return ChatRead(entry[tg_apih::sender_chat]);
    }

    // отправитель type == channel
    ChatRead chat() {
        return ChatRead(entry[tg_apih::chat]);
    }
};

}  // namespace fb