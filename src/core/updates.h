#pragma once
#include <Arduino.h>

#include "packet.h"
#include "keys.h"

namespace fb {

class Updates {
   public:
    enum Type : uint16_t {
        message = (1 << 0),
        edited_message = (1 << 1),
        channel_post = (1 << 2),
        edited_channel_post = (1 << 3),
        inline_query = (1 << 4),
        chosen_inline_result = (1 << 5),
        callback_query = (1 << 6),
        shipping_query = (1 << 7),
        pre_checkout_query = (1 << 8),
        poll = (1 << 9),
        poll_answer = (1 << 10),
        my_chat_member = (1 << 11),
        chat_member = (1 << 12),
        chat_join_request = (1 << 13),
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

    void fill(fb::packet& p) {
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
            if (read(i)) p.addStr(upd_arr[i]);
        }
    }

   private:
    uint16_t updates = 0xffff;
};

}  // namespace fb