#pragma once
#include <Arduino.h>

#include "api.h"
#include "packet.h"

namespace fb {

class Updates {
   public:
    enum Type : uint16_t {
        message = (1 << 0),
        editedMessage = (1 << 1),
        channelPost = (1 << 2),
        editedChannelPost = (1 << 3),
        inlineQuery = (1 << 4),
        chosenInlineResult = (1 << 5),
        callbackQuery = (1 << 6),
        shippingQuery = (1 << 7),
        preCheckoutQuery = (1 << 8),
        poll = (1 << 9),
        pollAnswer = (1 << 10),
        myChatMember = (1 << 11),
        chatMember = (1 << 12),
        chatJoinRequest = (1 << 13),
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