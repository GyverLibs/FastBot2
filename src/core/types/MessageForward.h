#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "Message.h"
#include "VirtualFastBot2_class.h"
#include "core/api.h"
#include "core/packet.h"

namespace fb {

// https://core.telegram.org/bots/api#forwardmessage
class MessageForward {
    friend class ::VirtualFastBot2;

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
    void makePacket(fb::Packet& p) const {
        p[fbapi::message_id()] = messageID;
        p[fbapi::from_chat_id()] = fromChatID;
        p[fbapi::chat_id()] = chatID;
        if (threadID >= 0) p[fbapi::message_thread_id()] = (threadID);
        if (!notification) p[fbapi::disable_notification()] = true;
        if (protect) p[fbapi::protect_content()] = true;
    }
};

}  // namespace fb