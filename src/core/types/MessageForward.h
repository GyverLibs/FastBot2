#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "Message.h"
#include "FastBot2Client_class.h"
#include "core/api.h"
#include "core/packet.h"

namespace fb {

// https://core.telegram.org/bots/api#forwardmessage
class MessageForward {
    friend class ::FastBot2Client;

   public:
    MessageForward() {}
    MessageForward(uint32_t messageID,
                   const su::Value& fromChatID,
                   const su::Value& chatID) : messageID(messageID),
                                              fromChatID(fromChatID),
                                              chatID(chatID) {}

    // id пересылаемого сообщения в чате
    uint32_t messageID;

    // id чата пересылаемого сообщения
    su::Value fromChatID;

    // id чата, в который пересылать
    su::Value chatID;

    // id темы в группе, в которую переслать
    int32_t threadID = -1;

    // уведомить о получении
    bool notification = Message::notificationDefault;

    // защитить от пересылки и копирования
    bool protect = Message::protectDefault;

   private:
    void makePacket(Packet& p) const {
        p[tg_api::message_id] = messageID;
        p[tg_api::from_chat_id] = fromChatID;
        p[tg_api::chat_id] = chatID;
        if (threadID >= 0) p[tg_api::message_thread_id] = (threadID);
        if (!notification) p[tg_api::disable_notification] = true;
        if (protect) p[tg_api::protect_content] = true;
    }
};

}  // namespace fb