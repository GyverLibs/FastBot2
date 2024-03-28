#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/api.h"
#include "core/config.h"
#include "core/packet.h"
#include "core/types/File.h"
#include "core/types/Location.h"
#include "core/types/Message.h"
#include "core/types/MessageEdit.h"
#include "core/types/MessageForward.h"
#include "core/types/MessageRead.h"
#include "core/types/QueryRead.h"
#include "core/types/Update.h"
#include "core/updates.h"

namespace fb {
// режим опроса обновлений
enum class Poll : uint8_t {
    Sync,   // синхронный (рекомендуемый период > 3500 мс)
    Async,  // асинхронный (рекомендуемый период > 3500 мс)
    Long,   // асинхронный long polling (рекомендуемый период > 20000 мс)
};
}  // namespace fb

class VirtualFastBot2 {
    typedef void (*CallbackResult)(gson::Entry& entry);
    typedef void (*CallbackRaw)(const su::Text& res);
    typedef void (*CallbackUpdate)(fb::Update& upd);

   public:
    // разрешение и запрет типов обновлений
    fb::Updates updates;

    VirtualFastBot2() {
        _token.reserve(47);
    }

    // ============================== SYSTEM ==============================

    // установить токен
    void setToken(const String& token) {
        _token = token;
    }

    // получить токен
    const String& getToken() {
        return _token;
    }

    // установить лимит - кол-во сообщений в одном обновлении (умолч. 3)
    void setLimit(uint8_t limit = 3) {
        _poll_limit = limit ? limit : 1;
    }

    // установить режим и период опроса (умолч. Poll::Sync и 4000 мс)
    void setPollMode(fb::Poll mode = fb::Poll::Sync, uint16_t period = 4000) {
        _poll_mode = mode;
        _poll_prd = period;
    }

    // получить режим опроса
    fb::Poll getPollMode() {
        return _poll_mode;
    }

    // установить таймаут ожидания ответа сервера (умолч. 3000 мс)
    void setTimeout(uint16_t timeout) {
        clientTimeout = timeout;
    }

    // запустить (по умолчанию уже запущен)
    void begin() {
        _state = true;
    }

    // остановить
    void end() {
        _state = false;
        clientStop();
    }

    // пропустить непрочитанные сообщения. Вызывать однократно
    void skipUpdates() {
        _poll_offset = -1;
    }

    // пропустить следующее сообщение (сдвинуть оффсет на 1)
    void skipNextMessage() {
        _poll_offset++;
    }

    // id последнего отправленного сообщения от бота
    uint32_t lastBotMessage() {
        return _last_bot;
    }

    // последняя отправленная ботом команда
    size_t lastCmd() {
        return _lastCmd;
    }

    // ============================== ATTACH ==============================

    // подключить обработчик обновлений вида void cb(fb::Update& u) {}
    void attachUpdate(CallbackUpdate callback) {
        _cbu1 = callback;
    }

    // отключить обработчик обновлений
    void detachUpdate() {
        _cbu1 = nullptr;
    }

    // подключить второй обработчик обновлений вида void cb(fb::Update& u) {}
    void attachUpdate2(CallbackUpdate callback) {
        _cbu2 = callback;
    }

    // отключить второй обработчик обновлений
    void detachUpdate2() {
        _cbu2 = nullptr;
    }

    // подключить обработчик результата вида void cb(gson::Entry& r) {}
    // здесь нельзя отправлять сообщения с флагом wait
    void attachResult(CallbackResult callback) {
        _cbr = callback;
    }

    // отключить обработчик результата
    void detachResult() {
        _cbr = nullptr;
    }

    // подключить обработчик сырых json данных Telegram вида void cb(const String& r) {}
    void attachRaw(CallbackRaw callback) {
        _cbs = callback;
    }

    // отключить обработчик сырых данных
    void detachRaw() {
        _cbs = nullptr;
    }

    // ============================== TICK ==============================

    // тикер, вызывать в loop
    void tick() {
        if (!_state) return;

        clientTick();

        if (clientWaiting()) {
            if (millis() - _last_send >= (_poll_wait ? (_poll_prd + clientTimeout) : clientTimeout)) {
                _poll_wait = 0;
                clientStop();
            }
        } else {
            if (millis() - _last_send >= (_poll_mode == fb::Poll::Long ? FB_LONG_POLL_TOUT : _poll_prd)) {
                fb::Packet p;
                _lastCmd = p.beginCmd(fbcmd::getUpdates(), _token);
                if (_poll_mode == fb::Poll::Long) p[fbapi::timeout()] = (uint16_t)(_poll_prd / 1000);
                p[fbapi::limit()] = _poll_limit;
                p[fbapi::offset()] = _poll_offset;
                p.beginArr(fbapi::allowed_updates());
                updates.fill(p);
                p.endArr();

                if (_poll_mode == fb::Poll::Sync) sendPacket(p, true);
                else _poll_wait = sendPacket(p, false);
            }
        }
    }

    // система ждёт ответа с обновлениями
    bool isPolling() {
        return _poll_wait;
    }

    // ============================== SEND ==============================

    // ответить на callback. Можно указать текст и вызвать alert
    bool answerCallbackQuery(const su::Text& id, su::Text text = su::Text(), bool show_alert = false, bool wait = false) {
        _query_answ = true;
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::answerCallbackQuery(), _token);
        p[fbapi::callback_query_id()] = id;
        if (text.valid()) p.addStringEsc(fbapi::text(), text);
        if (show_alert) p[fbapi::show_alert()] = true;
        return sendPacket(p, wait);
    }

    // переслать сообщение
    bool forwardMessage(const fb::MessageForward& m, bool wait = false) {
        if (!m.chatID.valid() || !m.fromChatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::sendMessage(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // отправить сообщение
    bool sendMessage(const fb::Message& m, bool wait = false) {
        if (!m.text.length() || !m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::sendMessage(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // отправить геолокацию
    bool sendLocation(const fb::Location& m, bool wait = false) {
        if (!m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::sendLocation(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

// ============================== FILE ==============================
#ifndef FB_NO_FILE
    bool sendFile(const fb::File& m, bool wait = false) {
        fb::Packet p;
        _lastCmd = p.beginFile(m.multipart, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать медиа
    bool editFile(const fb::FileEdit& m, bool wait = false) {
        if (!m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginFile(m.multipart, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }
#endif

    // ============================== SET ==============================

    // отправить статус "набирает сообщение" на 5 секунд
    bool setTyping(su::Value chatID, su::Value threadID = su::Value(), bool wait = false) {
        if (!chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::sendChatAction(), _token);
        p[fbapi::chat_id()] = chatID;
        p[fbapi::action()] = F("typing");
        if (threadID.valid()) p[fbapi::message_thread_id()] = threadID;
        return sendPacket(p, wait);
    }

    // установить заголовок чата
    bool setChatTitle(su::Value chatID, su::Text title, bool wait = false) {
        if (!chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::setChatTitle(), _token);
        p[fbapi::chat_id()] = chatID;
        p.addStringEsc(fbapi::title(), title);
        return sendPacket(p, wait);
    }

    // установить описание чата
    bool setChatDescription(su::Value chatID, su::Text description, bool wait = false) {
        if (!chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::setChatDescription(), _token);
        p[fbapi::chat_id()] = chatID;
        p.addStringEsc(fbapi::description(), description);
        return sendPacket(p, wait);
    }

    // ============================== PIN ==============================

    // закрепить сообщение
    bool pinChatMessage(su::Value chatID, su::Value messageID, bool notify = true, bool wait = false) {
        if (!chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::pinChatMessage(), _token);
        p[fbapi::chat_id()] = chatID;
        p[fbapi::message_id()] = messageID;
        p[fbapi::disable_notification()] = notify;
        return sendPacket(p, wait);
    }

    // открепить сообщение
    bool unpinChatMessage(su::Value chatID, su::Value messageID, bool wait = false) {
        if (!chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::unpinChatMessage(), _token);
        p[fbapi::chat_id()] = chatID;
        p[fbapi::message_id()] = messageID;
        return sendPacket(p, wait);
    }

    // открепить все сообщения
    bool unpinAllChatMessages(su::Value chatID, bool wait = false) {
        if (!chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::unpinAllChatMessages(), _token);
        p[fbapi::chat_id()] = chatID;
        return sendPacket(p, wait);
    }

    // ============================== EDIT ==============================

    // редактировать текст
    bool editText(const fb::TextEdit& m, bool wait = false) {
        if (!m.text.length() || !m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::editMessageText(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать заголовок
    bool editCaption(const fb::CaptionEdit& m, bool wait = false) {
        if (!m.caption.length() || !m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::editMessageCaption(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать меню
    bool editMenu(const fb::MenuEdit& m, bool wait = false) {
        if (!m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::editMessageReplyMarkup(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать геолокацию
    bool editLocation(const fb::LocationEdit& m, bool wait = false) {
        if (!m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::editMessageLiveLocation(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // остановить геолокацию
    bool stopLocation(const fb::LocationStop& m, bool wait = false) {
        if (!m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::stopMessageLiveLocation(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // ============================== DELETE ==============================

    // удалить сообщение
    bool deleteMessage(su::Value chatID, su::Value messageID, bool wait = false) {
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::deleteMessage(), _token);
        p[fbapi::chat_id()] = chatID;
        p[fbapi::message_id()] = messageID;
        return sendPacket(p, wait);
    }

    // удалить сообщения
    bool deleteMessages(su::Value chatID, uint32_t* messageIDs, uint16_t amount, bool wait = false) {
        fb::Packet p;
        _lastCmd = p.beginCmd(fbcmd::deleteMessages(), _token);
        p[fbapi::chat_id()] = chatID;
        p.beginArr(fbapi::message_ids());
        for (uint16_t i = 0; i < amount; i++) p += messageIDs[i];
        p.endArr();
        return sendPacket(p, wait);
    }

    // ============================== MANUAL ==============================

    // начать пакет для ручной отправки в API
    fb::Packet beginPacket(const __FlashStringHelper* cmd) {
        fb::Packet p;
        _lastCmd = p.beginCmd(cmd, _token);
        return p;
    }

    // отправить пакет
    bool sendPacket(fb::Packet& packet, bool wait) {
        _last_send = millis();
        if (_poll_wait) clientStop();
        return clientSend(packet, wait);
    }

    // парсить json ответ сервера
    bool parsePacket(const su::Text& s, bool useYield = true) {
        _poll_wait = 0;
        if (_cbs) _cbs(s);

        gson::Parser json(20);
        if (!json.parse(s)) return 0;
        json.hashKeys();

        if (!json[fbh::ok]) return 0;

        gson::Entry result = json[fbh::result];
        if (!result.valid()) return 0;

        if (useYield) yield();

        // ============= UPDATES =============
        if (result.type() == gson::Type::Array) {
            uint8_t len = result.length();
            if (len) _poll_offset = result[0][fbh::update_id].toInt32() + len;

            for (uint8_t i = 0; i < len; i++) {
                gson::Entry upd = result[i][1];
                if (!upd.valid()) continue;

                size_t typeHash = upd.keyHash();
                fb::Update update(upd, typeHash);
                if (typeHash == fbh::callback_query) _query_answ = 0;

                if (_cbu1) _cbu1(update);
                if (_cbu2) _cbu2(update);

                if (typeHash == fbh::callback_query && !_query_answ) {
                    answerCallbackQuery(update.query().id());
                }
                if (useYield) yield();
            }
        } else {
            // ============= RESPONSE =============
            if (result.includes(fbh::message_id)) _last_bot = result[fbh::message_id];
            if (_cbr) _cbr(result);
        }
        return 1;
    }

   protected:
    virtual bool clientSend(fb::Packet& packet, bool wait) = 0;
    virtual bool clientWaiting() = 0;
    virtual void clientStop() {}
    virtual void clientTick() {}

    uint16_t clientTimeout = 3000;

    // ============================== PRIVATE ==============================
   private:
    bool _state = true;
    String _token;
    uint16_t _poll_prd = 4000;
    fb::Poll _poll_mode = fb::Poll::Sync;
    uint8_t _poll_limit = 3;
    int32_t _poll_offset = 0;
    bool _poll_wait = 0;
    bool _query_answ = 0;
    uint32_t _last_bot = 0;
    uint32_t _last_send = 0;
    size_t _lastCmd = 0;

    CallbackUpdate _cbu1 = nullptr;
    CallbackUpdate _cbu2 = nullptr;
    CallbackResult _cbr = nullptr;
    CallbackRaw _cbs = nullptr;
};