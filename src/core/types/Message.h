#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "InlineMenu.h"
#include "Menu.h"
#include "VirtualFastBot2_class.h"
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

// https://core.telegram.org/bots/api#sendmessage
class Message {
    friend class ::VirtualFastBot2;

   public:
    enum class Mode : uint8_t {
        Text,
        MarkdownV2,
        HTML,
    };
    Message() {}
    Message(const String& text, const su::Value& chatID) : text(text), chatID(chatID) {}

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
    void setInlineMenu(fb::InlineMenu& menu) {
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
    fb::InlineMenu* _menu_inline = nullptr;

   protected:
    void makePacket(fb::Packet& p) const {
        p[fb::api::chat_id] = chatID;
        if (text.length()) p.addStringEsc(fb::api::text, text);
        if (threadID >= 0) p[fb::api::message_thread_id] = threadID;
        if (reply.messageID >= 0) {
            p.beginObj(fb::api::reply_parameters);
            p[fb::api::message_id] = reply.messageID;
            if (reply.chatID.valid()) p[fb::api::chat_id] = reply.chatID;
            p.endObj();
        }
        if (!preview) {
            p.beginObj(fb::api::link_preview_options);
            p[fb::api::is_disabled] = true;
            p.endObj();
        }
        if (!notification) p[fb::api::disable_notification] = true;
        if (protect) p[fb::api::protect_content] = true;
        if (mode != fb::Message::Mode::Text) p[fb::api::parse_mode] = (mode == fb::Message::Mode::MarkdownV2 ? F("MarkdownV2") : F("HTML"));

        if (_remove_menu || _menu_inline || _menu) {
            p.beginObj(fb::api::reply_markup);
            makeMenu(p);
            p.endObj();
        }
    }

    void makeQS(fb::Packet& p) const {
        p.addQS(fb::api::chat_id, chatID);
        if (text.length()) p.addQS(fb::api::text, text);
        if (threadID >= 0) p.addQS(fb::api::message_thread_id, threadID);
        if (reply.messageID >= 0) {
            p.beginQS(fb::api::reply_parameters);
            p.beginObj();
            p[fb::api::message_id] = reply.messageID;
            if (reply.chatID.valid()) p[fb::api::chat_id] = reply.chatID;
            p.endObj();
            p.end();
        }
        if (!preview) {
            p.beginQS(fb::api::link_preview_options);
            p.beginObj();
            p[fb::api::is_disabled] = true;
            p.endObj();
            p.end();
        }
        if (!notification) p.addQS(fb::api::disable_notification, true);
        if (protect) p.addQS(fb::api::protect_content, true);
        if (mode != fb::Message::Mode::Text) p.addQS(fb::api::parse_mode, mode == (fb::Message::Mode::MarkdownV2) ? F("MarkdownV2") : F("HTML"));

        if (_remove_menu || _menu_inline || _menu) {
            p.beginQS(fb::api::reply_markup);
            p.beginObj();
            makeMenu(p);
            p.endObj();
            p.end();
        }
    }

    void makeMenu(fb::Packet& p) const {
        if (_remove_menu) p[fb::api::remove_keyboard] = true;
        else if (_menu_inline) _menu_inline->_toJson(p);
        else if (_menu) _menu->_toJson(p);
    }
};

}  // namespace fb