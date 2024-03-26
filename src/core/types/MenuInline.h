#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#inlinekeyboardbutton
struct MenuInline {
    MenuInline() {}
    MenuInline(const String& text, const String& data) : text(text), data(data) {}
    MenuInline(uint16_t reserve) {
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
    MenuInline& addButton(su::Text text, su::Text data = su::Text()) {
        text.addString(this->text);
        if (data.valid()) data.addString(this->data);
        else text.addString(this->data);
        this->text += ';';
        this->data += ';';
        return *this;
    }

    // перенести строку
    MenuInline& newRow() {
        if (text.length()) text[text.length() - 1] = '\n';
        return *this;
    }
};

}  // namespace fb