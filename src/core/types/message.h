#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "FastBot2_class.h"
#include "Menu.h"
#include "MenuInline.h"
#include "core/api.h"
#include "core/packet.h"

namespace fb {

// https://core.telegram.org/bots/api#replyparameters
struct ReplyParam {
    // id сообщения, на которое отвечаем
    int32_t messageID = -1;

    // id чата, в котором находится сообщение, на которое отвечаем
    su::Value chatID;
};

// https://core.telegram.org/bots/api#message
class Message {
    friend class ::FastBot2;

   public:
    enum class Mode : uint8_t {
        Text,
        MarkdownV2,
        HTML,
    };
    Message() {}
    Message(const String& text, su::Value chatID) : text(text), chatID(chatID) {}

    // текст сообщения
    String text;

    // id чата, куда отправлять
    su::Value chatID;

    // id темы в группе, куда отправлять
    int32_t threadID = -1;

    // параметры ответа на сообщение
    ReplyParam reply;

    // включить превью для ссылок
    bool preview = previewDefault;

    // уведомить о получении
    bool notification = notificationDefault;

    // защитить от пересылки и копирования
    bool protect = protectDefault;

    // режим текста: Text, MarkdownV2, HTML
    Mode mode = modeDefault;

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
    static bool previewDefault;

    // уведомить о получении (умолч. 1)
    static bool notificationDefault;

    // защитить от пересылки и копирования (умолч. 0)
    static bool protectDefault;

    // режим текста: Text, MarkdownV2, HTML (умолч. Text)
    static Mode modeDefault;

   private:
    bool _remove_menu = 0;
    fb::Menu* _menu = nullptr;
    fb::MenuInline* _menu_inline = nullptr;

   protected:
    void makePacket(fb::Packet& p) {
        p.addInt(fbapi::chat_id(), chatID);
        if (text.length()) p.addString(fbapi::text(), text);
        if (threadID >= 0) p.addInt(fbapi::message_thread_id(), threadID);
        if (reply.messageID >= 0) {
            p.beginObj(fbapi::reply_parameters());
            p.addInt(fbapi::message_id(), reply.messageID);
            if (reply.chatID.valid()) p.addInt(fbapi::chat_id(), reply.chatID);
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
                su::TextParser rows(_menu_inline->text, '\n');
                su::TextParser data(_menu_inline->data, ';');
                while (rows.parse()) {
                    su::TextParser cols(rows, ';');
                    p.beginArr();
                    while (cols.parse()) {
                        data.parse();
                        p.beginObj();
                        p.addString(fbapi::text(), cols);
                        // url or callback_data
                        if (data.startsWith(F("http://")) ||
                            data.startsWith(F("https://")) ||
                            data.startsWith(F("tg://"))) {
                            p.addString(fbapi::url(), data);
                        } else {
                            p.addString(fbapi::callback_data(), data);
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
                su::TextParser rows(_menu->text, '\n');
                while (rows.parse()) {
                    su::TextParser cols(rows, ';');
                    p.beginArr();
                    while (cols.parse()) p.addString(cols);
                    p.endArr();
                }
                p.endArr();
                if (_menu->persistent) p.addBool(fbapi::is_persistent(), true);
                if (_menu->resize) p.addBool(fbapi::resize_keyboard(), true);
                if (_menu->oneTime) p.addBool(fbapi::one_time_keyboard(), true);
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

}  // namespace fb