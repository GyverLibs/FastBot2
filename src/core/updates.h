#pragma once
#include <Arduino.h>

#include "VirtualFastBot2_class.h"
#include "api.h"
#include "packet.h"

namespace fb {

class Updates {
    friend class ::VirtualFastBot2;

   public:
    enum Type : uint16_t {
        Message = (1 << 0),
        EditedMessage = (1 << 1),
        ChannelPost = (1 << 2),
        EditedChannelPost = (1 << 3),
        InlineQuery = (1 << 4),
        ChosenInlineResult = (1 << 5),
        CallbackQuery = (1 << 6),
        ShippingQuery = (1 << 7),
        PreCheckoutQuery = (1 << 8),
        Poll = (1 << 9),
        PollAnswer = (1 << 10),
        MyChatMember = (1 << 11),
        ChatMember = (1 << 12),
        ChatJoinRequest = (1 << 13),
    };

    void set(uint16_t nmods) {
        updates |= nmods;
    }
    void clear(uint16_t nmods) {
        updates &= ~nmods;
    }

    void setAll() {
        updates = 0xffff;
    }
    void clearAll() {
        updates = 0;
    }

    bool read(Type m) {
        return updates & (uint16_t)m;
    }
    bool read(uint8_t idx) {
        return updates & (1 << idx);
    }

   private:
    uint16_t updates = 0xffff;

    void fill(fb::Packet& p) {
        if (updates == 0xffff) return;

        const __FlashStringHelper* upd_arr[] = {
            fb::api::message,
            fb::api::edited_message,
            fb::api::channel_post,
            fb::api::edited_channel_post,
            fb::api::inline_query,
            fb::api::chosen_inline_result,
            fb::api::callback_query,
            fb::api::shipping_query,
            fb::api::pre_checkout_query,
            fb::api::poll,
            fb::api::poll_answer,
            fb::api::my_chat_member,
            fb::api::chat_member,
            fb::api::chat_join_request,
        };

        for (uint8_t i = 0; i < 14; i++) {
            if (read(i)) p += upd_arr[i];
        }
    }
};

}  // namespace fb