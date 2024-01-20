#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "FastBot2_class.h"
#include "core/api.h"

namespace fb {

class Menu {
    friend class ::FastBot2;

   public:
    Menu() {}
    Menu(const String& text) : text(text) {}

    // надписи кнопок. Гор. разделитель - ;, верт. - \n (кнопка_1 ; кнопка_2 \n кнопка_3 ; кнопка_4)
    String text;

    // подсказка, показывается в поле ввода при открытой клавиатуре (до 64 символов)
    String placeholder;

    // принудительно показывать клавиатуру
    bool persistent = default_persistent;

    // уменьшить клавиатуру под количество кнопок
    bool resize = default_resize;

    // автоматически скрывать после нажатия
    bool one_time = default_one_time;

    // показывать только упомянутым в сообщении юзерам
    bool selective = default_selective;

    // добавить кнопку
    Menu& addButton(AnyText text) {
        text.addString(this->text);
        this->text += ';';
        return *this;
    }

    // перенести строку
    Menu& newRow() {
        if (text.length()) text[text.length() - 1] = '\n';
        return *this;
    }

    // ===================================

    // принудительно показывать клавиатуру (умолч. 0)
    static bool default_persistent;

    // уменьшить клавиатуру под количество кнопок (умолч. 0)
    static bool default_resize;

    // автоматически скрывать после нажатия (умолч. 0)
    static bool default_one_time;

    // показывать только упомянутым в сообщении юзерам (умолч. 0)
    static bool default_selective;

   private:
};

bool Menu::default_persistent = 0;
bool Menu::default_resize = 0;
bool Menu::default_one_time = 0;
bool Menu::default_selective = 0;

class MenuInline {
    friend class ::FastBot2;

   public:
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
    MenuInline& addButton(AnyText text, AnyText data = AnyText()) {
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

   private:
};

}  // namespace fb