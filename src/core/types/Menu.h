#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "Message_class.h"
#include "../api.h"
#include "../packet.h"

namespace fb {

// https://core.telegram.org/bots/api#replykeyboardmarkup
class Menu {
    friend class Message;

   public:
    Menu() {}
    Menu(Text text) {
        text.toString(this->text);
    }

    // надписи кнопок. Гор. разделитель - ;, верт. - \n (кнопка_1 ; кнопка_2 \n кнопка_3 ; кнопка_4)
    String text = "";

    // подсказка, показывается в поле ввода при открытой клавиатуре (до 64 символов)
    String placeholder = "";

    // принудительно показывать клавиатуру
    bool persistent = persistentDefault;

    // уменьшить клавиатуру под количество кнопок
    bool resize = resizeDefault;

    // автоматически скрывать после нажатия
    bool oneTime = oneTimeDefault;

    // показывать только упомянутым в сообщении юзерам
    bool selective = selectiveDefault;

    // добавить кнопку
    Menu& addButton(Text text) {
        if (_first) _first = false;
        else this->text += ';';

        text.addString(this->text);
        return *this;
    }

    // перенести строку
    Menu& newRow() {
        text += '\n';
        _first = true;
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

   private:
    bool _first = true;

    void _toJson(Packet& p) {
        p.beginArr(tg_api::keyboard);
        su::TextParser rows(text, '\n');
        while (rows.parse()) {
            su::TextParser cols(rows, ';');
            p.beginArr();
            while (cols.parse()) p.addStringEsc(cols);
            p.endArr();
        }
        p.endArr();
        if (persistent) p[tg_api::is_persistent] = true;
        if (resize) p[tg_api::resize_keyboard] = true;
        if (oneTime) p[tg_api::one_time_keyboard] = true;
        if (selective) p[tg_api::selective] = true;
        if (placeholder.length()) p.addStringEsc(tg_api::input_field_placeholder, placeholder);
    }
};

}  // namespace fb