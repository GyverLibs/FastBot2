#pragma once
#include <Arduino.h>
#include <GSON.h>

#include "MessageRead.h"
#include "QueryRead.h"
#include "core/BracketAccess.h"
#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#update
class Update : public BracketAccess {
   public:
    enum class Type : size_t {
        Message = fbh::api::message,
        EditedMessage = fbh::api::edited_message,
        ChannelPost = fbh::api::channel_post,
        EditedChannelPost = fbh::api::edited_channel_post,
        BusinessConnection = fbh::api::business_connection,
        BusinessMessage = fbh::api::business_message,
        EditedBusinessMessage = fbh::api::edited_business_message,
        DeletedBusinessMessages = fbh::api::deleted_business_messages,
        MessageReaction = fbh::api::message_reaction,
        MessageReactionCount = fbh::api::message_reaction_count,
        InlineQuery = fbh::api::inline_query,
        ChosenInlineResult = fbh::api::chosen_inline_result,
        CallbackQuery = fbh::api::callback_query,
        ShippingQuery = fbh::api::shipping_query,
        PreCheckoutQuery = fbh::api::pre_checkout_query,
        Poll = fbh::api::poll,
        PollAnswer = fbh::api::poll_answer,
        MyChatMember = fbh::api::my_chat_member,
        ChatMember = fbh::api::chat_member,
        ChatJoinRequest = fbh::api::chat_join_request,
        ChatBoost = fbh::api::chat_boost,
        RemovedChatBoost = fbh::api::removed_chat_boost,
    };

    Update(gson::Entry& entry, size_t type) : BracketAccess(entry), _type((Type)type) {}

    // тип апдейта
    Type type() {
        return _type;
    }

    // ================ QUERY ================

    // это query
    bool isQuery() {
        return _type == Type::CallbackQuery;
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
        return _type == Type::Message || _type == Type::EditedMessage;
    }

    // это пост в канале
    bool isPost() {
        return _type == Type::ChannelPost || _type == Type::EditedChannelPost;
    }

    // это отредактированное сообщение или отредактированный пост
    bool isEdited() {
        return _type == Type::EditedMessage || _type == Type::EditedChannelPost;
    }

   private:
    Type _type;
};

}  // namespace fb