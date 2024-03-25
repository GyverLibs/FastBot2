#pragma once
#include <Arduino.h>
#include <GSON.h>

#define FB_LONG_POLL_TOUT 300

#include "FastBot2_class.h"
#include "core/BotClient.h"
#include "core/api.h"
#include "core/packet.h"
#include "core/types/Message.h"
#include "core/types/MessageEdit.h"
#include "core/types/MessageForward.h"
#include "core/types/MessageRead.h"
#include "core/types/QueryRead.h"
#include "core/types/Update.h"
#include "core/updates.h"

#ifndef FB2_CUSTOM_CLIENT
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <WiFiClientSecure.h>
#endif
#endif

namespace fb {
typedef void (*CallbackResult)(gson::Entry& entry);
typedef void (*CallbackRaw)(const String& res);
}  // namespace fb

class FastBot2 {
   public:
    // режим опроса обновлений
    enum class Poll : uint8_t {
        Sync,   // синхронный (рекомендуемый период > 3500 мс)
        Async,  // асинхронный (рекомендуемый период > 3500 мс)
        Long,   // асинхронный long polling (рекомендуемый период > 10000 мс)
    };

    // разрешение и запрет типов обновлений
    fb::Updates updates;

    // передать клиента и запустить
    FastBot2(Client* client = nullptr) {
        if (client) setClient(client);
        else {
#if !defined(FB2_CUSTOM_CLIENT) && (defined(ESP8266) || defined(ESP32))
            setClient(&espclient);
            espclient.setInsecure();
            // client.setBufferSizes(512, 512);  // TODO
#endif
        }
        begin();
        _token.reserve(47);
    }

    // ============================== SYSTEM ==============================

    // установить клиента
    void setClient(Client* client) {
        _client.setClient(client);
    }

    // установить токен
    void setToken(const String& token) {
        _token = token;
    }

    // получить токен
    String getToken() {
        return _token;
    }

    // установить лимит - кол-во сообщений в одном обновлении (умолч. 3)
    void setLimit(uint8_t limit = 3) {
        _poll_limit = limit ? limit : 1;
    }

    // установить лимит размера сообщения от сервера, при превышении сообщение будет пропущено (умолч. 15000)
    void setMemLimit(uint16_t limit = 15000) {
        _client.setMemLimit(limit);
    }

    // установить режим и период опроса (умолч. Poll::Sync и 4000 мс)
    void setPollMode(Poll mode = Poll::Sync, uint16_t period = 4000) {
        _poll_mode = mode;
        _poll_tout = period;
    }

    // установить таймаут ожидания подключения (умолч. 3000 мс)
    void setTimeout(uint16_t timeout = 3000) {
        _client.setTimeout(timeout);
    }

    // запустить
    void begin() {
        _state = true;
    }

    // остановить
    void end() {
        _state = false;
        _client.stop();
    }

    // пропустить непрочитанные сообщения. Вызвать однократно
    void skipUpdates() {
        _poll_offset = -1;
    }

    // ============================== ATTACH ==============================

    // подключить обработчик обновлений вида void cb(fb::Update& u) {}
    void attachUpdate(fb::CallbackUpdate callback) {
        _cbu1 = callback;
    }

    // отключить обработчик обновлений
    void detachUpdate() {
        _cbu1 = nullptr;
    }

    // подключить второй обработчик обновлений вида void cb(fb::Update& u) {}
    void attachUpdate2(fb::CallbackUpdate callback) {
        _cbu2 = callback;
    }

    // отключить второй обработчик обновлений
    void detachUpdate2() {
        _cbu2 = nullptr;
    }

    // подключить обработчик результата вида void cb(gson::Entry& r) {}
    // здесь нельзя отправлять сообщения с флагом wait
    void attachResult(fb::CallbackResult callback) {
        _cbr = callback;
    }

    // отключить обработчик результата
    void detachResult() {
        _cbr = nullptr;
    }

    // подключить обработчик сырых json данных Telegram вида void cb(const String& r) {}
    void attachRaw(fb::CallbackRaw callback) {
        _cbs = callback;
    }

    // отключить обработчик сырых данных
    void detachRaw() {
        _cbs = nullptr;
    }

    // ============================== MISC ==============================

    // id последнего отправленного сообщения от бота. Для опроса сразу после sendMessage - ставь wait у sendMessage
    uint32_t lastBotMessage() {
        return _last_bot;
    }

    // ============================== TICK ==============================

    // тикер, вызывать в loop
    void tick() {
        if (!_state) return;

        if (_client.available()) {
            String s = _client.read(&_error);
            _parse(s);
        }

        if (_client.waiting()) {
            uint32_t prd = _poll_wait ? (_poll_tout + _client.timeout) : _client.timeout;
            if (millis() - _client.last_ms >= prd) {
                _poll_wait = 0;
                _client.stop();
            }
        } else {
            if (millis() - _client.last_ms >= (_poll_mode == Poll::Long ? FB_LONG_POLL_TOUT : _poll_tout)) {
                _getUpdates();
            }
        }
    }

    // система ждёт ответа с обновлениями
    bool waiting() {
        return _poll_wait;
    }

    // ============================== ERROR ==============================

    // есть ошибка
    bool hasError() {
        return _error != fb::Error::None;
    }

    // прочитать ошибку. Вызов сбросит ошибку
    fb::Error getError() {
        fb::Error buf = _error;
        _error = fb::Error::None;
        return buf;
    }

    // ============================== SEND ==============================

    // ответить на callback. Можно указать текст и вызвать alert
    bool answerCallbackQuery(su::Text id, su::Text text = su::Text(), bool show_alert = false) {
        _query_answ = true;
        fb::Packet p(fbcmd::answerCallbackQuery(), _token);
        p.addString(fbapi::callback_query_id(), id);
        if (text.valid()) p.addString(fbapi::text(), text);
        if (show_alert) p.addBool(fbapi::show_alert(), true);
        return sendPacket(p);
    }

    // переслать сообщение
    bool forwardMessage(fb::MessageForward& m, bool wait = false) {
        if (!m.chatID.valid() || !m.fromChatID.valid() || !m.messageID.valid()) return 0;
        fb::Packet p(fbcmd::sendMessage(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // отправить сообщение
    bool sendMessage(su::Text text, su::Value chat_id, bool wait = false) {
        fb::Message msg(text, chat_id);
        return sendMessage(msg, wait);
    }

    // отправить сообщение
    bool sendMessage(fb::Message& m, bool wait = false) {
        if (!m.text.length() || !m.chatID.valid()) return 0;
        fb::Packet p(fbcmd::sendMessage(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // ============================== EDIT ==============================

    // редактировать текст
    bool editText(fb::EditText& m, bool wait = false) {
        if (!m.text.length() || !m.chatID.valid() || m.messageID < 0) return 0;
        fb::Packet p(fbcmd::editMessageText(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать заголовок
    bool editCaption(fb::EditCaption& m, bool wait = false) {
        if (!m.caption.length() || !m.chatID.valid() || m.messageID < 0) return 0;
        fb::Packet p(fbcmd::editMessageCaption(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать клавиатуру
    bool editKeyboard(fb::EditKeyboard& m, bool wait = false) {
        if (!m.chatID.valid() || m.messageID < 0) return 0;
        fb::Packet p(fbcmd::editMessageReplyMarkup(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать медиа
    bool editMedia(fb::EditMedia& m, bool wait = false) {
        if (!m.chatID.valid() || m.messageID < 0) return 0;
        fb::Packet p(fbcmd::editMessageMedia(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // редактировать геолокацию
    bool editLocation(fb::EditLocation& m, bool wait = false) {
        if (!m.chatID.valid() || m.messageID < 0) return 0;
        fb::Packet p(fbcmd::editMessageLiveLocation(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // остановить геолокацию
    bool stopLocation(fb::StopLocation& m, bool wait = false) {
        if (!m.chatID.valid() || m.messageID < 0) return 0;
        fb::Packet p(fbcmd::stopMessageLiveLocation(), _token);
        m.makePacket(p);
        return sendPacket(p, wait);
    }

    // ============================== DELETE ==============================

    // удалить сообщение
    bool deleteMessage(su::Value chatID, uint32_t messageID, bool wait = false) {
        fb::Packet p(fbcmd::deleteMessage(), _token);
        p.addInt(fbapi::chat_id(), chatID);
        p.addInt(fbapi::message_id(), messageID);
        return sendPacket(p, wait);
    }

    // удалить сообщения
    bool deleteMessages(su::Value chatID, uint32_t* messageIDs, uint16_t amount, bool wait = false) {
        fb::Packet p(fbcmd::deleteMessages(), _token);
        p.addInt(fbapi::chat_id(), chatID);
        p.beginArr(fbapi::message_ids());
        for (uint16_t i = 0; i < amount; i++) p.addInt(messageIDs[i]);
        p.endArr();
        return sendPacket(p, wait);
    }

    // ============================== CUSTOM ==============================

    // начать пакет для ручной отправки в API
    fb::Packet beginPacket(const __FlashStringHelper* cmd) {
        return fb::Packet(cmd, _token);
    }

    // отправить данные
    bool sendPacket(fb::Packet& p, bool wait = false) {
        // wait resp Packet
        while (!_poll_wait && _client.waiting()) {
            if (_client.available()) {
                String s = _client.read(&_error);
                _parse(s);
            }
            yield();
        }
        _poll_wait = 0;
        // if (!_last_bot) wait = true;

        if (wait && _allow_send_wait) {
            String s = _client.send_read(p, &_error);
            _parse(s);
            return !hasError();
        } else {
            _client.send(p);
            return 1;
        }
    }

    // подключен ли клиент
    bool _connected() {
        return _client.connected();
    }

#ifndef FB2_CUSTOM_CLIENT
#if defined(ESP8266)
    BearSSL::WiFiClientSecure espclient;
#elif defined(ESP32)
    WiFiClientSecure espclient;
#endif
#endif

    // ============================== PRIVATE ==============================
   private:
    BotClient _client;
    bool _state = 0;
    fb::Error _error = fb::Error::None;
    String _token;

    uint16_t _poll_tout = 4000;
    Poll _poll_mode = Poll::Sync;
    uint8_t _poll_limit = 3;
    int32_t _poll_offset = 0;
    bool _poll_wait = 0;
    bool _allow_send_wait = 1;
    bool _query_answ = 0;

    uint32_t _last_bot = 0;

    fb::CallbackUpdate _cbu1 = nullptr;
    fb::CallbackUpdate _cbu2 = nullptr;
    fb::CallbackResult _cbr = nullptr;
    fb::CallbackRaw _cbs = nullptr;

    // ==============================================
    void _getUpdates() {
        fb::Packet p(fbcmd::getUpdates(), _token);
        if (_poll_mode == Poll::Long) p.addInt(fbapi::timeout(), (uint16_t)(_poll_tout / 1000));
        p.addInt(fbapi::limit(), _poll_limit);
        p.addInt(fbapi::offset(), _poll_offset);
        p.beginArr(fbapi::allowed_updates());
        updates.fill(p);
        p.endArr();

        if (_poll_mode == Poll::Sync) {
            String s = _client.send_read(p, &_error);
            _parse(s);
        } else {
            _poll_wait = _client.send(p);
        }
    }

    // ==============================================
    fb::Error _parse(String& s) {
        _client.last_ms = millis();
        _poll_wait = 0;

        switch (_error) {
            case fb::Error::None:
                break;

            case fb::Error::MemLimit:
            case fb::Error::Reserve:
                _poll_offset = -1;
            default:
                return _error;
        }
        yield();

        if (_cbs) _cbs(s);
        yield();

        gson::Parser json(20);
        if (!json.parse(s)) return _error = fb::Error::Parse;
        yield();

        json.hashKeys();
        if (!json[fbhash::ok]) return _error = fb::Error::Telegram;

        gson::Entry result = json[fbhash::result];
        if (!result.valid()) return _error = fb::Error::Telegram;

        // ============= UPDATES =============
        if (result.type() == gson::Type::Array) {
            uint8_t len = result.length();
            if (len) _poll_offset = result[0][fbhash::update_id].toInt32() + len;

            for (uint8_t i = 0; i < len; i++) {
                gson::Entry upd = result[i][1];
                if (!upd.valid()) continue;

                size_t typeHash = upd.keyHash();
                fb::Update update(upd, typeHash);
                if (typeHash == fbhash::callback_query) _query_answ = 0;

                if (_cbu1) _cbu1(update);
                if (_cbu2) _cbu2(update);

                if (typeHash == fbhash::callback_query && !_query_answ) {
                    answerCallbackQuery(update.query().id());
                }
                yield();
            }
        } else {
            // ============= RESPONSE =============
            _allow_send_wait = 0;
            if (result.includes(fbhash::message_id)) _last_bot = result[fbhash::message_id];
            if (_cbr) _cbr(result);
            _allow_send_wait = 1;
        }

        return fb::Error::None;
    }
};