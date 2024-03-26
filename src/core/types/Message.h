#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "FastBot2_class.h"
#include "Menu.h"
#include "InlineMenu.h"
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
    friend class ::FastBot2;

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
        p.addInt(fbapi::chat_id(), chatID);
        if (text.length()) p.addStringEsc(fbapi::text(), text);
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
            makeMenu(p);
            p.endObj();
        }
    }

    void makeQS(fb::Packet& p) const {
        p.addQS(fbapi::chat_id(), chatID);
        if (text.length()) p.addQS(fbapi::text(), text);
        if (threadID >= 0) p.addQS(fbapi::message_thread_id(), threadID);
        if (reply.messageID >= 0) {
            p.beginQS(fbapi::reply_parameters());
            p.beginObj();
            p.addInt(fbapi::message_id(), reply.messageID);
            if (reply.chatID.valid()) p.addInt(fbapi::chat_id(), reply.chatID);
            p.endObj();
            p.end();
        }
        if (!preview) {
            p.beginQS(fbapi::link_preview_options());
            p.beginObj();
            p.addBool(fbapi::is_disabled(), true);
            p.endObj();
            p.end();
        }
        if (!notification) p.addQS(fbapi::disable_notification(), true);
        if (protect) p.addQS(fbapi::protect_content(), true);
        if (mode != fb::Message::Mode::Text) p.addQS(fbapi::parse_mode(), mode == (fb::Message::Mode::MarkdownV2) ? F("MarkdownV2") : F("HTML"));

        if (_remove_menu || _menu_inline || _menu) {
            p.beginQS(fbapi::reply_markup());
            p.beginObj();
            makeMenu(p);
            p.endObj();
            p.end();
        }
    }

    void makeMenu(fb::Packet& p) const {
        if (_remove_menu) p.addBool(fbapi::remove_keyboard(), true);
        else if (_menu_inline) _menu_inline->_toJson(p);
        else if (_menu) _menu->_toJson(p);
    }
};

}  // namespace fb