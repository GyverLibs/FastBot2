#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "FastBot2_class.h"
#include "core/api.h"
#include "menu.h"

namespace fb {

using sutil::AnyValue;

class Message {
    friend class ::FastBot2;

   public:
    enum class Mode : uint8_t {
        Text,
        MarkdownV2,
        HTML,
    };
    Message() {}
    Message(const String& text, AnyValue chat_id) : text(text), chat_id(chat_id) {}

    // текст сообщения
    String text;

    // id чата, куда отправлять
    AnyValue chat_id;

    // id темы в группе
    int32_t thread_id = -1;

    // ответить на сообщение с указанным id
    int32_t reply_to = -1;

    // включить превью для ссылок
    bool preview = default_preview;

    // уведомить о получении
    bool notification = default_notification;

    // защитить от пересылки и копирования
    bool protect = default_protect;

    // режим текста: Text, MarkdownV2, HTML
    Mode mode = default_mode;

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

    // ===================================

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

// ===================================================

struct MessageForward {
    MessageForward() {}
    MessageForward(AnyValue message_id, AnyValue from_chat_id, AnyValue chat_id) : message_id(message_id), from_chat_id(from_chat_id), chat_id(chat_id) {}

    // id пересылаемого сообщения в чате
    AnyValue message_id;

    // id чата пересылаемого сообщения
    AnyValue from_chat_id;

    // id чата, в который пересылать
    AnyValue chat_id;

    // id темы в группе, в которую переслать
    int32_t thread_id = -1;

    // уведомить о получении
    bool notification = Message::default_notification;

    // защитить от пересылки и копирования
    bool protect = Message::default_protect;
};

}  // namespace fb