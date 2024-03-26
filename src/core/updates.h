#pragma once
#include <Arduino.h>

#include "api.h"
#include "packet.h"

namespace fb {

class Updates {
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

    void fill(fb::Packet& p) {
        if (updates == 0xffff) return;

        const __FlashStringHelper* upd_arr[] = {
            fbapi::message(),
            fbapi::edited_message(),
            fbapi::channel_post(),
            fbapi::edited_channel_post(),
            fbapi::inline_query(),
            fbapi::chosen_inline_result(),
            fbapi::callback_query(),
            fbapi::shipping_query(),
            fbapi::pre_checkout_query(),
            fbapi::poll(),
            fbapi::poll_answer(),
            fbapi::my_chat_member(),
            fbapi::chat_member(),
            fbapi::chat_join_request(),
        };

        for (uint8_t i = 0; i < 14; i++) {
            if (read(i)) p.addString(upd_arr[i]);
        }
    }

   private:
    uint16_t updates = 0xffff;
};

}  // namespace fb