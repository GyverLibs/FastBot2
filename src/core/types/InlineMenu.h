#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/api.h"
#include "core/packet.h"

namespace fb {

// https://core.telegram.org/bots/api#inlinekeyboardbutton
struct InlineMenu {
    InlineMenu() {}
    InlineMenu(const String& text, const String& data) : text(text), data(data) {}
    InlineMenu(uint16_t reserve) {
        this->reserve(reserve);
    }

    // надписи кнопок. Гор. разделитель - ;, верт. - \n (кнопка_1 ; кнопка_2 \n кнопка_3 ; кнопка_4)
    String text;

    // callback data кнопок с разделителем ; . Поддерживаются url адреса
    String data;

    // зарезервировать строки
    void reserve(uint16_t len) {
        text.reserve(len);
        data.reserve(len);
    }

    // добавить кнопку
    InlineMenu& addButton(su::Text text, su::Text data = su::Text()) {
        text.addString(this->text);
        if (data.valid()) data.addString(this->data);
        else text.addString(this->data);
        this->text += ';';
        this->data += ';';
        return *this;
    }

    // перенести строку
    InlineMenu& newRow() {
        if (text.length()) text[text.length() - 1] = '\n';
        return *this;
    }

    void _toJson(Packet& p) {
        p.beginArr(fbapi::inline_keyboard());
        _trim(text);
        _trim(data);
        su::TextParser rows(text, '\n');
        su::TextParser datap(data, ';');
        while (rows.parse()) {
            su::TextParser cols(rows, ';');
            p.beginArr();
            while (cols.parse()) {
                datap.parse();
                p.beginObj();
                p.addStringEsc(fbapi::text(), cols);
                // url or callback_data
                if (datap.startsWith(F("http://")) ||
                    datap.startsWith(F("https://")) ||
                    datap.startsWith(F("tg://"))) {
                    p[fbapi::url()] = datap;
                } else {
                    p.addStringEsc(fbapi::callback_data(), datap);
                }
                p.endObj();
            }
            p.endArr();
        }
        p.endArr();
    }

   private:
    void _trim(String& s) const {
        if (s[s.length() - 1] == ';') s.remove(s.length() - 1);
    }
};

}  // namespace fb