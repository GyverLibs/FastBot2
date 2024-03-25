#pragma once
#include <Arduino.h>
#include <GSON.h>

#include "MessageRead.h"
#include "QueryRead.h"
#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#update
class Update {
   public:
    enum class Type : size_t {
        message = fbhash::message,
        editedMessage = fbhash::edited_message,
        channelPost = fbhash::channel_post,
        editedChannelPost = fbhash::edited_channel_post,
        inlineQuery = fbhash::inline_query,
        chosenInlineResult = fbhash::chosen_inline_result,
        callbackQuery = fbhash::callback_query,
        shippingQuery = fbhash::shipping_query,
        preCheckoutQuery = fbhash::pre_checkout_query,
        poll = fbhash::poll,
        pollAnswer = fbhash::poll_answer,
        myChatMember = fbhash::my_chat_member,
        chatMember = fbhash::chat_member,
        chatJoinRequest = fbhash::chat_join_request,
    };

    Update(gson::Entry& entry, size_t type) : entry(entry), _type((Type)type) {}

    // тип апдейта
    Type type() {
        return _type;
    }

    // ================ QUERY ================

    // это query
    bool isQuery() {
        return _type == Type::callbackQuery;
    }

    // query
    QueryRead query() {
        return QueryRead(entry);
    }

    // ================ MESSAGE ================

    // сообщение
    MessageRead message() {
        return MessageRead(entry);
    }

    // это сообщение
    bool isMessage() {
        return _type == Type::message || _type == Type::editedMessage;
    }

    // это пост в канале
    bool isPost() {
        return _type == Type::channelPost || _type == Type::editedChannelPost;
    }

    // это отредактированное сообщение или отредактированный пост
    bool isEdited() {
        return _type == Type::editedMessage || _type == Type::editedChannelPost;
    }

    // доступ к пакету данных
    gson::Entry& entry;

   private:
    Type _type;
};

typedef void (*CallbackUpdate)(Update& upd);

}  // namespace fb