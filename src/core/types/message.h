#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "FastBot2_class.h"
#include "core/api.h"
#include "core/packet.h"
#include "menu.h"

namespace fb {

using sutil::AnyValue;

struct ReplyParam {
    // id сообщения, на которое отвечаем
    int32_t message_id = -1;

    // id чата, в котором находится сообщение, на которое отвечаем
    AnyValue chat_id;
};

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

    // id темы в группе, куда отправлять
    int32_t thread_id = -1;

    // параметры ответа на сообщение
    ReplyParam reply;

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

    void makePacket(fb::Packet& p) {
        p.addString(fbapi::text(), text);
        p.addInt(fbapi::chat_id(), chat_id);
        if (thread_id >= 0) p.addInt(fbapi::message_thread_id(), thread_id);
        if (reply.message_id >= 0) {
            p.beginObj(fbapi::reply_parameters());
            p.addInt(fbapi::message_id(), reply.message_id);
            if (reply.chat_id.valid()) p.addInt(fbapi::chat_id(), reply.chat_id);
            p.endObj();
        }
        if (!preview) {
            p.beginObj(fbapi::link_preview_options());
            p.addBool(fbapi::is_disabled(), true);
            p.endObj();
        }
        if (!notification) p.addBool(fbapi::disable_notification(), true);
        if (protect) p.addBool(fbapi::protect_content(), true);
        if (mode != fb::Message::Mode::Text) p.addString(fbapi::parse_mode(), mode == (fb::Message::Mode::MarkdownV2) ? F("MarkdownV2") : F("HTML"));
        if (_remove_menu || _menu_inline || _menu) {
            p.beginObj(fbapi::reply_markup());
            // REMOVE MENU
            if (_remove_menu) {
                p.addBool(fbapi::remove_keyboard(), true);

                // INLINE MENU
            } else if (_menu_inline) {
                p.beginArr(fbapi::inline_keyboard());
                _trim(_menu_inline->text);
                _trim(_menu_inline->data);
                sutil::Parser rows(_menu_inline->text, '\n');
                sutil::Parser data(_menu_inline->data, ';');
                while (rows.next()) {
                    sutil::Parser cols(rows.str(), ';');
                    p.beginArr();
                    while (cols.next()) {
                        data.next();
                        p.beginObj();
                        p.addString(fbapi::text(), cols.str());
                        // url or callback_data
                        if (!strncmp_P(data.str(), PSTR("http://"), 7) ||
                            !strncmp_P(data.str(), PSTR("https://"), 8) ||
                            !strncmp_P(data.str(), PSTR("tg://"), 5)) {
                            p.addString(fbapi::url(), data.str());
                        } else {
                            p.addString(fbapi::callback_data(), data.str());
                        }
                        p.endObj();
                    }
                    p.endArr();
                }
                p.endArr();

                // REPLY MENU
            } else {
                p.beginArr(fbapi::keyboard());
                _trim(_menu->text);
                sutil::Parser rows(_menu->text, '\n');
                while (rows.next()) {
                    sutil::Parser cols(rows.str(), ';');
                    p.beginArr();
                    while (cols.next()) p.addString(cols.str());
                    p.endArr();
                }
                p.endArr();
                if (_menu->persistent) p.addBool(fbapi::is_persistent(), true);
                if (_menu->resize) p.addBool(fbapi::resize_keyboard(), true);
                if (_menu->one_time) p.addBool(fbapi::one_time_keyboard(), true);
                if (_menu->selective) p.addBool(fbapi::selective(), true);
                if (_menu->placeholder.length()) p.addString(fbapi::input_field_placeholder(), _menu->placeholder);
            }
            p.endObj();
        }
    }

    void _trim(String& s) {
        if (s[s.length() - 1] == ';') s.remove(s.length() - 1);
    }
};

bool Message::default_preview = 1;
bool Message::default_notification = 1;
bool Message::default_protect = 0;
Message::Mode Message::default_mode = Message::Mode::Text;

// ===================================================

class MessageForward {
    friend class ::FastBot2;

   public:
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

   private:
    void makePacket(fb::Packet& p) {
        p.addInt(fbapi::message_id(), message_id);
        p.addInt(fbapi::from_chat_id(), from_chat_id);
        p.addInt(fbapi::chat_id(), chat_id);
        if (thread_id >= 0) p.addInt(fbapi::message_thread_id(), thread_id);
        if (!notification) p.addBool(fbapi::disable_notification(), true);
        if (protect) p.addBool(fbapi::protect_content(), true);
    }
};

}  // namespace fb