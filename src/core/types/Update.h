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
        message = fbh::api::message,
        editedMessage = fbh::api::edited_message,
        channelPost = fbh::api::channel_post,
        editedChannelPost = fbh::api::edited_channel_post,
        inlineQuery = fbh::api::inline_query,
        chosenInlineResult = fbh::api::chosen_inline_result,
        callbackQuery = fbh::api::callback_query,
        shippingQuery = fbh::api::shipping_query,
        preCheckoutQuery = fbh::api::pre_checkout_query,
        poll = fbh::api::poll,
        pollAnswer = fbh::api::poll_answer,
        myChatMember = fbh::api::my_chat_member,
        chatMember = fbh::api::chat_member,
        chatJoinRequest = fbh::api::chat_join_request,
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



}  // namespace fb