#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/Fetcher.h"
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
    typedef void (*CallbackFetch)(Stream& stream);

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
        _cbUpdate1 = callback;
    }

    // отключить обработчик обновлений
    void detachUpdate() {
        _cbUpdate1 = nullptr;
    }

    // подключить второй обработчик обновлений вида void cb(fb::Update& u) {}
    void attachUpdate2(CallbackUpdate callback) {
        _cbUpdate2 = callback;
    }

    // отключить второй обработчик обновлений
    void detachUpdate2() {
        _cbUpdate2 = nullptr;
    }

    // подключить обработчик результата вида void cb(gson::Entry& r) {}
    // здесь нельзя отправлять сообщения с флагом wait
    void attachResult(CallbackResult callback) {
        _cbResult = callback;
    }

    // отключить обработчик результата
    void detachResult() {
        _cbResult = nullptr;
    }

    // подключить обработчик сырых json данных Telegram вида void cb(const String& r) {}
    void attachRaw(CallbackRaw callback) {
        _cbRaw = callback;
    }

    // отключить обработчик сырых данных
    void detachRaw() {
        _cbRaw = nullptr;
    }

    // подключить обработчик скачивания файлов вида void cb(Stream& stream, size_t length) {}
    void attachFetch(CallbackFetch callback) {
        _cbFetch = callback;
    }

    // отключить обработчик скачивания файлов
    void detachFetch() {
        _cbFetch = nullptr;
    }

    // ============================== TICK ==============================

    // тикер, вызывать в loop
    bool tick() {
        if (!_state) return 0;

        if (clientWaiting()) {
            if (millis() - _last_send >= (_poll_wait ? (_poll_prd + clientTimeout) : clientTimeout)) {
                _poll_wait = 0;
                clientStop();
            }
        } else {
            if (millis() - _last_send >= (_poll_mode == fb::Poll::Long ? FB_LONG_POLL_TOUT : _poll_prd)) {
                getUpdates(false);
            }
        }

        return clientTick();
    }

    // система ждёт ответа с обновлениями
    bool isPolling() {
        return _poll_wait;
    }

    // отправить запрос на обновление
    bool getUpdates(bool wait = false) {
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::getUpdates, _token);
        if (_poll_mode == fb::Poll::Long) p[fb::api::timeout] = (uint16_t)(_poll_prd / 1000);
        p[fb::api::limit] = _poll_limit;
        p[fb::api::offset] = _poll_offset;
        p.beginArr(fb::api::allowed_updates);
        updates.fill(p);
        p.endArr();

        if (_poll_mode == fb::Poll::Sync || wait) return sendPacket(p, true);
        else return _poll_wait = sendPacket(p, false);
    }

    // запросить перезагрузку устройства
    void reboot() {
        _reboot = fb::Fetcher::Reboot::Triggered;
    }

    // можно перезагрузить устройство
    bool canReboot() {
        return _reboot == fb::Fetcher::Reboot::CanReboot;
    }

    // ============================== SEND ==============================

    // ответить на callback. Можно указать текст и вызвать alert
    bool answerCallbackQuery(const su::Text& id, su::Text text = su::Text(), bool show_alert = false, bool wait = false) {
        _query_answ = true;
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::answerCallbackQuery, _token);
        p[fb::api::callback_query_id] = id;
        if (text.valid()) p.addStringEsc(fb::api::text, text);
        if (show_alert) p[fb::api::show_alert] = true;
        return sendPacket(p, wait);
    }

    // переслать сообщение
    bool forwardMessage(const fb::MessageForward& m, bool wait = false) {
        if (!m.chatID.valid() || !m.fromChatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::sendMessage, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // отправить сообщение
    bool sendMessage(const fb::Message& m, bool wait = false) {
        if (!m.text.length() || !m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::sendMessage, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // отправить геолокацию
    bool sendLocation(const fb::Location& m, bool wait = false) {
        if (!m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::sendLocation, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

// ============================== FILE ==============================
#ifndef FB_NO_FILE
    bool sendFile(const fb::File& m, bool wait = false) {
        fb::Packet p;
        _lastCmd = p.beginMultipart(m.multipart, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать медиа
    bool editFile(const fb::FileEdit& m, bool wait = false) {
        if (!m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginMultipart(m.multipart, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // запросить файл (придёт в обработчик attachFetch)
    bool getFile(const su::Text& fileID, bool wait = false) {
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::getFile, _token);
        p[fb::api::file_id] = fileID;
        _lastFileID = fileID.hash();
        return sendPacket(p, wait);
    }

    // скачать файл
    fb::Fetcher downloadFile(const su::Text& fileID) {
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::getFile, _token);
        p[fb::api::file_id] = fileID;
        _lastFileID = fileID.hash();

        fb::Fetcher fetcher(&_reboot);
        _fetcher = &fetcher;
        sendPacket(p, true);
        _fetcher = nullptr;
        return fetcher;
    }
#endif

    // ============================== SET ==============================

    // отправить статус "набирает сообщение" на 5 секунд
    bool setTyping(su::Value chatID, bool wait = false) {
        if (!chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::sendChatAction, _token);
        p[fb::api::chat_id] = chatID;
        p[fb::api::action] = F("typing");
        return sendPacket(p, wait);
    }

    // установить заголовок чата
    bool setChatTitle(su::Value chatID, su::Text title, bool wait = false) {
        if (!chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::setChatTitle, _token);
        p[fb::api::chat_id] = chatID;
        p.addStringEsc(fb::api::title, title);
        return sendPacket(p, wait);
    }

    // установить описание чата
    bool setChatDescription(su::Value chatID, su::Text description, bool wait = false) {
        if (!chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::setChatDescription, _token);
        p[fb::api::chat_id] = chatID;
        p.addStringEsc(fb::api::description, description);
        return sendPacket(p, wait);
    }

    // ============================== PIN ==============================

    // закрепить сообщение
    bool pinChatMessage(su::Value chatID, su::Value messageID, bool notify = true, bool wait = false) {
        if (!chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::pinChatMessage, _token);
        p[fb::api::chat_id] = chatID;
        p[fb::api::message_id] = messageID;
        p[fb::api::disable_notification] = notify;
        return sendPacket(p, wait);
    }

    // открепить сообщение
    bool unpinChatMessage(su::Value chatID, su::Value messageID, bool wait = false) {
        if (!chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::unpinChatMessage, _token);
        p[fb::api::chat_id] = chatID;
        p[fb::api::message_id] = messageID;
        return sendPacket(p, wait);
    }

    // открепить все сообщения
    bool unpinAllChatMessages(su::Value chatID, bool wait = false) {
        if (!chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::unpinAllChatMessages, _token);
        p[fb::api::chat_id] = chatID;
        return sendPacket(p, wait);
    }

    // ============================== EDIT ==============================

    // редактировать текст
    bool editText(const fb::TextEdit& m, bool wait = false) {
        if (!m.text.length() || !m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::editMessageText, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать заголовок
    bool editCaption(const fb::CaptionEdit& m, bool wait = false) {
        if (!m.caption.length() || !m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::editMessageCaption, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать меню
    bool editMenu(const fb::MenuEdit& m, bool wait = false) {
        if (!m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::editMessageReplyMarkup, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать геолокацию
    bool editLocation(const fb::LocationEdit& m, bool wait = false) {
        if (!m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::editMessageLiveLocation, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // остановить геолокацию
    bool stopLocation(const fb::LocationStop& m, bool wait = false) {
        if (!m.chatID.valid()) return 0;
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::stopMessageLiveLocation, _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // ============================== DELETE ==============================

    // удалить сообщение
    bool deleteMessage(su::Value chatID, su::Value messageID, bool wait = false) {
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::deleteMessage, _token);
        p[fb::api::chat_id] = chatID;
        p[fb::api::message_id] = messageID;
        return sendPacket(p, wait);
    }

    // удалить сообщения
    bool deleteMessages(su::Value chatID, uint32_t* messageIDs, uint16_t amount, bool wait = false) {
        fb::Packet p;
        _lastCmd = p.beginJson(fb::cmd::deleteMessages, _token);
        p[fb::api::chat_id] = chatID;
        p.beginArr(fb::api::message_ids);
        for (uint16_t i = 0; i < amount; i++) p += messageIDs[i];
        p.endArr();
        return sendPacket(p, wait);
    }

    // ============================== MANUAL ==============================

    // начать пакет для ручной отправки в API
    fb::Packet beginPacket(const __FlashStringHelper* cmd) {
        fb::Packet p;
        _lastCmd = p.beginJson(cmd, _token);
        return p;
    }

    // отправить пакет
    bool sendPacket(fb::Packet& packet, bool wait) {
        _last_send = millis();
        if (_poll_wait) clientStop();
        return clientSend(packet, wait);
    }

    // принять файл в коллбэк
    void handleFile(Stream& stream) {
        if (_cbFetch) _cbFetch(stream);
    }

    // парсить json ответ сервера
    bool parsePacket(const su::Text& s, bool useYield = true) {
        _poll_wait = 0;
        if (_cbRaw) _cbRaw(s);

        gson::Parser json(20);
        if (!json.parse(s)) return 0;
        json.hashKeys();

        if (!json[fbh::api::ok]) return 0;

        gson::Entry result = json[fbh::api::result];
        if (!result.valid()) return 0;

        if (useYield) yield();

        // ============= UPDATES =============
        if (result.type() == gson::Type::Array) {
            if (_reboot == fb::Fetcher::Reboot::WaitUpdate) {
                _reboot = fb::Fetcher::Reboot::CanReboot;
#ifdef FB_ESP_BUILD
                ESP.restart();
#endif
                return 1;
            }
            uint8_t len = result.length();
            if (len) _poll_offset = result[0][fbh::api::update_id].toInt32();

            for (uint8_t i = 0; i < len; i++) {
                _poll_offset++;
                gson::Entry upd = result[i][1];
                if (!upd.valid()) continue;

                size_t typeHash = upd.keyHash();
                fb::Update update(upd, typeHash);
                if (typeHash == fbh::api::callback_query) _query_answ = 0;

                if (_cbUpdate1) _cbUpdate1(update);
                if (_cbUpdate2) _cbUpdate2(update);

                if (typeHash == fbh::api::callback_query && !_query_answ) {
                    answerCallbackQuery(update.query().id());
                }
                if (useYield) yield();
                if (_reboot == fb::Fetcher::Reboot::Triggered) {
                    _reboot = fb::Fetcher::Reboot::WaitUpdate;
                    return 1;
                }
            }
        } else {
            // ============= RESPONSE =============
            if (result.includes(fbh::api::message_id)) _last_bot = result[fbh::api::message_id];
            if (result.includes(fbh::api::file_id) && result[fbh::api::file_id].hash() == _lastFileID) {
                _lastFileID = 0;
                fb::Packet p;
                p.beginDownload(result[fbh::api::file_path], _token);
                if (_fetcher) {
                    clientStream(p, &_fetcher->_stream);
                } else {
                    sendPacket(p, 0);
                }
            }
            if (_cbResult) _cbResult(result);
        }
        return 1;
    }

   protected:
    virtual bool clientSend(fb::Packet& packet, bool wait) = 0;
    virtual bool clientStream(fb::Packet& packet, Stream** stream) = 0;
    virtual bool clientWaiting() { return 0; }
    virtual bool clientTick() { return 0; }
    virtual void clientStop() {}

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
    size_t _lastFileID = 0;
    fb::Fetcher* _fetcher = nullptr;
    fb::Fetcher::Reboot _reboot = fb::Fetcher::Reboot::Idle;

    CallbackUpdate _cbUpdate1 = nullptr;
    CallbackUpdate _cbUpdate2 = nullptr;
    CallbackResult _cbResult = nullptr;
    CallbackRaw _cbRaw = nullptr;
    CallbackFetch _cbFetch = nullptr;
};