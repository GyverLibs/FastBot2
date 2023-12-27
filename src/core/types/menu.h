#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/keys.h"

namespace fb {

void _trimDiv(String& s) {
    if (s[s.length() - 1] == ';') s.remove(s.length() - 1);
}

struct Menu {
    Menu(const String& text = String()) : text(text) {
        persistent = default_persistent;
        resize = default_resize;
        one_time = default_one_time;
        selective = default_selective;
    }

    // надписи кнопок. Гор. разделитель - ;, верт. - \n (кнопка_1 ; кнопка_2 \n кнопка_3 ; кнопка_4)
    String text;

    // подсказка, показывается в поле ввода при открытой клавиатуре (до 64 символов)
    String placeholder;

    // принудительно показывать клавиатуру
    bool persistent;

    // уменьшить клавиатуру под количество кнопок
    bool resize;

    // автоматически скрывать после нажатия
    bool one_time;

    // показывать только упомянутым в сообщении юзерам
    bool selective;

    // принудительно показывать клавиатуру (умолч. 0)
    static bool default_persistent;

    // уменьшить клавиатуру под количество кнопок (умолч. 0)
    static bool default_resize;

    // автоматически скрывать после нажатия (умолч. 0)
    static bool default_one_time;

    // показывать только упомянутым в сообщении юзерам (умолч. 0)
    static bool default_selective;

    // добавить кнопку
    Menu& addButton(AnyText text) {
        text.toString(this->text);
        this->text += ';';
        return *this;
    }

    // перенести строку
    Menu& newRow() {
        if (text.length()) text[text.length() - 1] = '\n';
        return *this;
    }

    void _trim() {
        _trimDiv(text);
    }
};

bool Menu::default_persistent = 0;
bool Menu::default_resize = 0;
bool Menu::default_one_time = 0;
bool Menu::default_selective = 0;

struct MenuInline {
    MenuInline(const String& text = String(), const String& data = String()) : text(text), data(data) {}
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
    MenuInline& addButton(AnyText text, AnyText data = AnyText()) {
        text.toString(this->text);
        if (data.valid()) data.toString(this->data);
        else text.toString(this->data);
        this->text += ';';
        this->data += ';';
        return *this;
    }

    // перенести строку
    MenuInline& newRow() {
        if (text.length()) text[text.length() - 1] = '\n';
        return *this;
    }

    void _trim() {
        _trimDiv(text);
        _trimDiv(data);
    }
};

}  // namespace fb