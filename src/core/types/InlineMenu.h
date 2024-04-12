#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "Message_class.h"
#include "core/api.h"
#include "core/packet.h"

namespace fb {

// https://core.telegram.org/bots/api#inlinekeyboardbutton
class InlineMenu {
    friend class Message;

   public:
    InlineMenu() {}
    InlineMenu(const String& text, const String& data) : text(text), data(data) {}
    InlineMenu(uint16_t reserve) {
        this->reserve(reserve);
    }

    // надписи кнопок. Гор. разделитель - ;, верт. - \n (кнопка_1 ; кнопка_2 \n кнопка_3 ; кнопка_4)
    String text = "";

    // callback data кнопок с разделителем ; . Поддерживаются url адреса
    String data = "";

    // зарезервировать строки
    void reserve(uint16_t len) {
        text.reserve(len);
        data.reserve(len);
    }

    // добавить кнопку
    InlineMenu& addButton(su::Text text, su::Text data = su::Text()) {
        if (_first) _first = false;
        else {
            this->text += ';';
            this->data += ';';
        }

        text.addString(this->text);
        if (data.valid()) data.addString(this->data);
        else text.addString(this->data);
        return *this;
    }

    // перенести строку
    InlineMenu& newRow() {
        if (text.length()) text[text.length() - 1] = '\n';
        _first = true;
        return *this;
    }

   private:
    bool _first = true;

    void _toJson(Packet& p) {
        p.beginArr(api::inline_keyboard);
        su::TextParser rows(text, '\n');
        su::TextParser datap(data, ';');
        while (rows.parse()) {
            su::TextParser cols(rows, ';');
            p.beginArr();
            while (cols.parse()) {
                datap.parse();
                p.beginObj();
                p.addStringEsc(api::text, cols);
                // url or callback_data
                if (datap.startsWith(F("http://")) ||
                    datap.startsWith(F("https://")) ||
                    datap.startsWith(F("tg://"))) {
                    p[api::url] = datap;
                } else {
                    p.addStringEsc(api::callback_data, datap);
                }
                p.endObj();
            }
            p.endArr();
        }
        p.endArr();
    }
};

}  // namespace fb