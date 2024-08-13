#pragma once
#include <Arduino.h>

#include "api.h"
#include "core_class.h"
#include "packet.h"

#define FB_UPDATES_AMOUNT 22
#define FB_UPDATES_FILL (bit(FB_UPDATES_AMOUNT) - 1)

namespace fb {

class Updates {
    friend class Core;

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

    // установить
    void set(uint32_t nmods) {
        updates |= nmods;
    }

    // очистить
    void clear(uint32_t nmods) {
        updates &= ~nmods;
    }

    // включить все
    void setAll() {
        updates = FB_UPDATES_FILL;
    }

    // очистить все
    void clearAll() {
        updates = 0;
    }

    // прочитать по типу
    bool read(Type m) {
        return updates & (uint32_t)m;
    }

    // прочитать по индексу
    bool read(uint8_t idx) {
        return updates & (1 << idx);
    }

   private:
    uint32_t updates = FB_UPDATES_FILL;

    void makePacket(Packet& p) {
        if (updates == FB_UPDATES_FILL) return;

        const __FlashStringHelper* upd_arr[] = {
            tg_api::message,
            tg_api::edited_message,
            tg_api::channel_post,
            tg_api::edited_channel_post,
            tg_api::business_connection,
            tg_api::business_message,
            tg_api::edited_business_message,
            tg_api::deleted_business_messages,
            tg_api::message_reaction,
            tg_api::message_reaction_count,
            tg_api::inline_query,
            tg_api::chosen_inline_result,
            tg_api::callback_query,
            tg_api::shipping_query,
            tg_api::pre_checkout_query,
            tg_api::poll,
            tg_api::poll_answer,
            tg_api::my_chat_member,
            tg_api::chat_member,
            tg_api::chat_join_request,
            tg_api::chat_boost,
            tg_api::removed_chat_boost,
        };

        p.beginArr(tg_api::allowed_updates);
        for (uint8_t i = 0; i < FB_UPDATES_AMOUNT; i++) {
            if (read(i)) p += upd_arr[i];
        }
        p.endArr();
    }
};

}  // namespace fb