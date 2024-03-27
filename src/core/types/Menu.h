#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/api.h"
#include "core/packet.h"

namespace fb {

// https://core.telegram.org/bots/api#replykeyboardmarkup
struct Menu {
    Menu() {}
    Menu(const String& text) : text(text) {}

    // надписи кнопок. Гор. разделитель - ;, верт. - \n (кнопка_1 ; кнопка_2 \n кнопка_3 ; кнопка_4)
    String text;

    // подсказка, показывается в поле ввода при открытой клавиатуре (до 64 символов)
    String placeholder;

    // принудительно показывать клавиатуру
    bool persistent = persistentDefault;

    // уменьшить клавиатуру под количество кнопок
    bool resize = resizeDefault;

    // автоматически скрывать после нажатия
    bool oneTime = oneTimeDefault;

    // показывать только упомянутым в сообщении юзерам
    bool selective = selectiveDefault;

    // добавить кнопку
    Menu& addButton(su::Text text) {
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
    static bool persistentDefault;

    // уменьшить клавиатуру под количество кнопок (умолч. 0)
    static bool resizeDefault;

    // автоматически скрывать после нажатия (умолч. 0)
    static bool oneTimeDefault;

    // показывать только упомянутым в сообщении юзерам (умолч. 0)
    static bool selectiveDefault;

    void _toJson(Packet& p) {
        p.beginArr(fbapi::keyboard());
        _trim(text);
        su::TextParser rows(text, '\n');
        while (rows.parse()) {
            su::TextParser cols(rows, ';');
            p.beginArr();
            while (cols.parse()) p.addStringEsc(cols);
            p.endArr();
        }
        p.endArr();
        if (persistent) p[fbapi::is_persistent()] = true;
        if (resize) p[fbapi::resize_keyboard()] = true;
        if (oneTime) p[fbapi::one_time_keyboard()] = true;
        if (selective) p[fbapi::selective()] = true;
        if (placeholder.length()) p.addStringEsc(fbapi::input_field_placeholder(), placeholder);
    }

   private:
    void _trim(String& s) const {
        if (s[s.length() - 1] == ';') s.remove(s.length() - 1);
    }
};

}  // namespace fb