#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "FastBot2_class.h"
#include "core/keys.h"
#include "menu.h"

namespace fb {

class Message {
    friend class ::FastBot2;

   public:
    enum class Mode : uint8_t {
        Text,
        MarkdownV2,
        HTML,
    };

    Message(const String& text = String(), sutil::AnyValue chat_id = sutil::AnyValue()) : text(text), chat_id(chat_id) {
        preview = default_preview;
        notification = default_notification;
        protect = default_protect;
        mode = default_mode;
    }

    // текст сообщения
    String text;

    // id чата, куда отправлять
    sutil::AnyValue chat_id;

    // id темы в группе
    int32_t thread_id = -1;

    // ответить на сообщение с указанным id
    int32_t reply_to = -1;

    // включить превью для ссылок
    bool preview;

    // уведомить о получении
    bool notification;

    // защитить от пересылки и копирования
    bool protect;

    // режим текста: Text, MarkdownV2, HTML
    Mode mode;

    // добавить обычное меню
    void setMenu(fb::Menu& menu) {
        if (menu.text.length()) _menu = &menu;
    }

    // добавить инлайн меню
    void setMenu(fb::MenuInline& menu) {
        if (menu.text.length() && menu.data.length()) _menu_inline = &menu;
    }

    // удалить обычное меню
    void removeMenu() {
        _remove_menu = 1;
    }

    // включить превью для ссылок (умолч. 1)
    static bool default_preview;

    // уведомить о получении (умолч. 1)
    static bool default_notification;

    // защитить от пересылки и копирования (умолч. 0)
    static bool default_protect;

    // режим текста: Text, MarkdownV2, HTML (умолч. Text)
    static Mode default_mode;

   private:
    bool _remove_menu = 0;
    fb::Menu* _menu = nullptr;
    fb::MenuInline* _menu_inline = nullptr;
};

bool Message::default_preview = 1;
bool Message::default_notification = 1;
bool Message::default_protect = 0;
Message::Mode Message::default_mode = Message::Mode::Text;

}  // namespace fb