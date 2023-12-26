#pragma once
#include <Arduino.h>
#include <GSON.h>

#include "core/keys.h"
#include "messageRead.h"

namespace fb {

class Update {
   public:
    enum class Type : size_t {
        message = fbhash::message,
        edited_message = fbhash::edited_message,
        channel_post = fbhash::channel_post,
        edited_channel_post = fbhash::edited_channel_post,

        inline_query = fbhash::inline_query,
        chosen_inline_result = fbhash::chosen_inline_result,
        callback_query = fbhash::callback_query,
        shipping_query = fbhash::shipping_query,
        pre_checkout_query = fbhash::pre_checkout_query,
        poll = fbhash::poll,
        poll_answer = fbhash::poll_answer,
        my_chat_member = fbhash::my_chat_member,
        chat_member = fbhash::chat_member,
        chat_join_request = fbhash::chat_join_request,
    };

    Update(gson::Entry& entry, size_t type) : entry(entry), _type((Type)type) {}

    // тип апдейта
    Type type() {
        return _type;
    }

    // ================ MESSAGE ================

    // сообщение
    MessageRead message() {
        return MessageRead(entry);
    }

    // это сообщение
    bool isMessage() {
        return _type == Type::message || _type == Type::edited_message;
    }

    // это пост в канале
    bool isPost() {
        return _type == Type::channel_post || _type == Type::edited_channel_post;
    }

    // это отредактированное сообщение или отредактированный пост
    bool isEdited() {
        return _type == Type::edited_message || _type == Type::edited_channel_post;
    }

    // доступ к пакету данных
    gson::Entry& entry;

   private:
    Type _type;
};

typedef void (*CallbackUpdate)(Update& upd);

}  // namespace fb