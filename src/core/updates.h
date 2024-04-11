#pragma once
#include <Arduino.h>

#include "VirtualFastBot2_class.h"
#include "api.h"
#include "packet.h"

#define FB_UPDATES_AMOUNT 22
#define FB_UPDATES_FILL (bit(FB_UPDATES_AMOUNT) - 1)

namespace fb {

class Updates {
    friend class ::VirtualFastBot2;

   public:
    enum Type : uint32_t {
        Message = bit(0),
        EditedMessage = bit(1),
        ChannelPost = bit(2),
        EditedChannelPost = bit(3),
        BusinessConnection = bit(4),
        BusinessMessage = bit(5),
        EditedBusinessMessage = bit(6),
        DeletedBusinessMessages = bit(7),
        MessageReaction = bit(8),
        MessageReactionCount = bit(9),
        InlineQuery = bit(10),
        ChosenInlineResult = bit(11),
        CallbackQuery = bit(12),
        ShippingQuery = bit(13),
        PreCheckoutQuery = bit(14),
        Poll = bit(15),
        PollAnswer = bit(16),
        MyChatMember = bit(17),
        ChatMember = bit(18),
        ChatJoinRequest = bit(19),
        ChatBoost = bit(20),
        RemovedChatBoost = bit(21),
    };

    void set(uint32_t nmods) {
        updates |= nmods;
    }
    void clear(uint32_t nmods) {
        updates &= ~nmods;
    }

    void setAll() {
        updates = FB_UPDATES_FILL;
    }
    void clearAll() {
        updates = 0;
    }

    bool read(Type m) {
        return updates & (uint32_t)m;
    }
    bool read(uint8_t idx) {
        return updates & (1 << idx);
    }

   private:
    uint32_t updates = FB_UPDATES_FILL;

    void fill(Packet& p) {
        if (updates == FB_UPDATES_FILL) return;

        const __FlashStringHelper* upd_arr[] = {
            api::message,
            api::edited_message,
            api::channel_post,
            api::edited_channel_post,
            api::business_connection,
            api::business_message,
            api::edited_business_message,
            api::deleted_business_messages,
            api::message_reaction,
            api::message_reaction_count,
            api::inline_query,
            api::chosen_inline_result,
            api::callback_query,
            api::shipping_query,
            api::pre_checkout_query,
            api::poll,
            api::poll_answer,
            api::my_chat_member,
            api::chat_member,
            api::chat_join_request,
            api::chat_boost,
            api::removed_chat_boost,
        };

        for (uint8_t i = 0; i < FB_UPDATES_AMOUNT; i++) {
            if (read(i)) p += upd_arr[i];
        }
    }
};

}  // namespace fb