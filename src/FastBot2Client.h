#pragma once
#include "core/core.h"
#include "core/types/File.h"
#include "core/types/Location.h"
#include "core/types/Message.h"
#include "core/types/MessageEdit.h"
#include "core/types/MessageForward.h"
#include "core/types/MessageRead.h"
#include "core/types/MyCommands.h"
#include "core/types/QueryRead.h"

class FastBot2Client : public fb::Core {
   public:
    using fb::Core::Core;

    // ============================== SEND ==============================

    // ответить на callback. Можно указать текст и вызвать alert
    fb::Result answerCallbackQuery(const Text& id, Text text = Text(), bool show_alert = false, bool wait = true) {
        _query_answ = true;
        fb::Packet p(tg_cmd::answerCallbackQuery, _token);
        p[tg_api::callback_query_id] = id;
        if (text) p.addStringEsc(tg_api::text, text);
        if (show_alert) p[tg_api::show_alert] = true;
        return sendPacket(p, wait);
    }

    // переслать сообщение
    fb::Result forwardMessage(const fb::MessageForward& m, bool wait = true) {
        if (!m.chatID || !m.fromChatID) return fb::Result();
        fb::Packet p(tg_cmd::forwardMessage, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // отправить сообщение
    fb::Result sendMessage(const fb::Message& m, bool wait = true) {
        if (!m.text.length() || !m.chatID) return fb::Result();
        fb::Packet p(tg_cmd::sendMessage, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // отправить геолокацию
    fb::Result sendLocation(const fb::Location& m, bool wait = true) {
        if (!m.chatID) return fb::Result();
        fb::Packet p(tg_cmd::sendLocation, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

// ============================== FILE ==============================
#ifndef FB_NO_FILE
    // отправить файл, тип указывается в fb::File
    fb::Result sendFile(const fb::File& m, bool wait = true) {
        fb::Packet p(m.multipart, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать файл
    fb::Result editFile(const fb::FileEdit& m, bool wait = true) {
        if (!m.chatID) return fb::Result();
        fb::Packet p(m.multipart, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // обновить прошивку из файла, указать id юзера для отправки уведомления
    void updateFlash(fb::DocumentRead document, Text user_id = "") {
        _ota = ota_t::Flash;
        document.id().toString(_ota_id);
        user_id.toString(_ota_user);
    }

    // обновить FS из файла, указать id юзера для отправки уведомления
    void updateFS(fb::DocumentRead document, Text user_id = "") {
        _ota = ota_t::FS;
        document.id().toString(_ota_id);
        user_id.toString(_ota_user);
    }

    // скачать файл
    fb::Fetcher downloadFile(fb::DocumentRead document) {
        return fb::Core::downloadFile(document.id());
    }

    using fb::Core::downloadFile;
#endif

    // ============================== SET ==============================

    // отправить статус "набирает сообщение" на 5 секунд
    fb::Result setTyping(Value chatID, bool wait = true) {
        if (!chatID) return fb::Result();
        fb::Packet p(tg_cmd::sendChatAction, _token);
        p[tg_api::chat_id] = chatID;
        p[tg_api::action] = F("typing");
        return sendPacket(p, wait);
    }

    // установить заголовок чата
    fb::Result setChatTitle(Value chatID, Text title, bool wait = true) {
        if (!chatID) return fb::Result();
        fb::Packet p(tg_cmd::setChatTitle, _token);
        p[tg_api::chat_id] = chatID;
        p.addStringEsc(tg_api::title, title);
        return sendPacket(p, wait);
    }

    // установить описание чата
    fb::Result setChatDescription(Value chatID, Text description, bool wait = true) {
        if (!chatID) return fb::Result();
        fb::Packet p(tg_cmd::setChatDescription, _token);
        p[tg_api::chat_id] = chatID;
        p.addStringEsc(tg_api::description, description);
        return sendPacket(p, wait);
    }

    // установить подсказки команд бота
    fb::Result setMyCommands(const fb::MyCommands& commands, bool wait = true) {
        fb::Packet p(tg_cmd::setMyCommands, _token);
        commands.makePacket(p);
        return sendPacket(p, wait);
    }

    // удалить подсказки команд бота
    fb::Result deleteMyCommands(bool wait = true) {
        fb::Packet p(tg_cmd::deleteMyCommands, _token);
        return sendPacket(p, wait);
    }

    // установить имя бота
    fb::Result setMyName(const Text& name, bool wait = true) {
        fb::Packet p(tg_cmd::setMyName, _token);
        p[tg_api::name] = name;
        return sendPacket(p, wait);
    }

    // установить описание бота
    fb::Result setMyDescription(const Text& description, bool wait = true) {
        fb::Packet p(tg_cmd::setMyDescription, _token);
        p[tg_api::description] = description;
        return sendPacket(p, wait);
    }

    // ============================== PIN ==============================

    // закрепить сообщение
    fb::Result pinChatMessage(Value chatID, Value messageID, bool notify = true, bool wait = true) {
        if (!chatID) return fb::Result();
        fb::Packet p(tg_cmd::pinChatMessage, _token);
        p[tg_api::chat_id] = chatID;
        p[tg_api::message_id] = messageID;
        p[tg_api::disable_notification] = notify;
        return sendPacket(p, wait);
    }

    // открепить сообщение
    fb::Result unpinChatMessage(Value chatID, Value messageID, bool wait = true) {
        if (!chatID) return fb::Result();
        fb::Packet p(tg_cmd::unpinChatMessage, _token);
        p[tg_api::chat_id] = chatID;
        p[tg_api::message_id] = messageID;
        return sendPacket(p, wait);
    }

    // открепить все сообщения
    fb::Result unpinAllChatMessages(Value chatID, bool wait = true) {
        if (!chatID) return fb::Result();
        fb::Packet p(tg_cmd::unpinAllChatMessages, _token);
        p[tg_api::chat_id] = chatID;
        return sendPacket(p, wait);
    }

    // ============================== EDIT ==============================

    // редактировать текст
    // https://core.telegram.org/bots/api#editmessagetext
    fb::Result editText(const fb::TextEdit& m, bool wait = true) {
        if (!m.text.length() || !m.chatID) return fb::Result();
        fb::Packet p(tg_cmd::editMessageText, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать заголовок (у сообщений с медиафайлом)
    // https://core.telegram.org/bots/api#editmessagecaption
    fb::Result editCaption(const fb::CaptionEdit& m, bool wait = true) {
        if (!m.caption.length() || !m.chatID) return fb::Result();
        fb::Packet p(tg_cmd::editMessageCaption, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать меню
    // https://core.telegram.org/bots/api#editmessagereplymarkup
    fb::Result editMenu(const fb::MenuEdit& m, bool wait = true) {
        if (!m.chatID) return fb::Result();
        fb::Packet p(tg_cmd::editMessageReplyMarkup, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать геолокацию
    fb::Result editLocation(const fb::LocationEdit& m, bool wait = true) {
        if (!m.chatID) return fb::Result();
        fb::Packet p(tg_cmd::editMessageLiveLocation, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // остановить геолокацию
    fb::Result stopLocation(const fb::LocationStop& m, bool wait = true) {
        if (!m.chatID) return fb::Result();
        fb::Packet p(tg_cmd::stopMessageLiveLocation, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // ============================== DELETE ==============================

    // удалить сообщение
    fb::Result deleteMessage(Value chatID, Value messageID, bool wait = true) {
        fb::Packet p(tg_cmd::deleteMessage, _token);
        p[tg_api::chat_id] = chatID;
        p[tg_api::message_id] = messageID;
        return sendPacket(p, wait);
    }

    // удалить сообщения
    fb::Result deleteMessages(Value chatID, uint32_t* messageIDs, uint16_t amount, bool wait = true) {
        fb::Packet p(tg_cmd::deleteMessages, _token);
        p[tg_api::chat_id] = chatID;
        p.beginArr(tg_api::message_ids);
        for (uint16_t i = 0; i < amount; i++) p += messageIDs[i];
        p.endArr();
        return sendPacket(p, wait);
    }

   private:
};