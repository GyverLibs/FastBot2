#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "../api.h"
#include "../packet.h"
#include "FastBot2Client_class.h"
#include "InlineMenu.h"
#include "Menu.h"
#include "Message_class.h"

namespace fb {

// https://core.telegram.org/bots/api#replyparameters
struct ReplyParam {
    // id сообщения, на которое отвечаем
    int32_t messageID = -1;

    // id чата, в котором находится сообщение, на которое отвечаем
    Value chatID;
};

// https://core.telegram.org/bots/api#sendmessage
class Message {
    friend class ::FastBot2Client;

   public:
    enum class Mode : uint8_t {
        Text,
        MarkdownV2,
        HTML,
    };
    Message() {}
    Message(Text text, Value chatID) : chatID(chatID) {
        text.toString(this->text);
    }

    // для ручного добавления тех параметров, которых нет в классе!
    gson::string json;

    // текст сообщения
    String text;

    // id чата, куда отправлять
    Value chatID;

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
    void setMenu(Menu& menu) {
        if (menu.text.length()) _menu = &menu;
    }

    // добавить инлайн меню
    void setInlineMenu(InlineMenu& menu) {
        if (menu.text.length() && menu.data.length()) _menu_inline = &menu;
    }

    // удалить обычное меню
    void removeMenu() {
        _remove_menu = 1;
    }

    // установить режим текста Markdown
    void setModeMD() {
        mode = Mode::MarkdownV2;
    }

    // установить режим текста HTML
    void setModeHTML() {
        mode = Mode::HTML;
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
    Menu* _menu = nullptr;
    InlineMenu* _menu_inline = nullptr;

   protected:
    void makePacket(Packet& p) const {
        p[tg_api::chat_id] = chatID;
        if (text.length()) p.addStringEsc(tg_api::text, text);
        if (threadID >= 0) p[tg_api::message_thread_id] = threadID;
        if (reply.messageID >= 0) {
            p.beginObj(tg_api::reply_parameters);
            p[tg_api::message_id] = reply.messageID;
            if (reply.chatID) p[tg_api::chat_id] = reply.chatID;
            p.endObj();
        }
        if (!preview) {
            p.beginObj(tg_api::link_preview_options);
            p[tg_api::is_disabled] = true;
            p.endObj();
        }
        if (!notification) p[tg_api::disable_notification] = true;
        if (protect) p[tg_api::protect_content] = true;
        if (mode != Message::Mode::Text) p[tg_api::parse_mode] = (mode == Message::Mode::MarkdownV2 ? F("MarkdownV2") : F("HTML"));

        if (_remove_menu || _menu_inline || _menu) {
            p.beginObj(tg_api::reply_markup);
            makeMenu(p);
            p.endObj();
        }

        p += json;
    }

    void makeQS(Packet& p) const {
        p.addQS(tg_api::chat_id, chatID);
        if (text.length()) p.addQS(tg_api::text, text);
        if (threadID >= 0) p.addQS(tg_api::message_thread_id, threadID);
        if (reply.messageID >= 0) {
            p.beginQS(tg_api::reply_parameters);
            p.beginObj();
            p[tg_api::message_id] = reply.messageID;
            if (reply.chatID) p[tg_api::chat_id] = reply.chatID;
            p.endObj();
            p.end();
        }
        if (!preview) {
            p.beginQS(tg_api::link_preview_options);
            p.beginObj();
            p[tg_api::is_disabled] = true;
            p.endObj();
            p.end();
        }
        if (!notification) p.addQS(tg_api::disable_notification, true);
        if (protect) p.addQS(tg_api::protect_content, true);
        if (mode != Message::Mode::Text) p.addQS(tg_api::parse_mode, mode == (Message::Mode::MarkdownV2) ? F("MarkdownV2") : F("HTML"));

        if (_remove_menu || _menu_inline || _menu) {
            p.beginQS(tg_api::reply_markup);
            p.beginObj();
            makeMenu(p);
            p.endObj();
            p.end();
        }
    }

    void makeMenu(Packet& p) const {
        if (_remove_menu) p[tg_api::remove_keyboard] = true;
        else if (_menu_inline) _menu_inline->_toJson(p);
        else if (_menu) _menu->_toJson(p);
    }
};

}  // namespace fb