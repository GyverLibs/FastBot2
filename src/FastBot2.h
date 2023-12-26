#pragma once
#include <Arduino.h>
#include <GSON.h>

#define FB_LONG_POLL_TOUT 300

#include "core/BotClient.h"
#include "core/keys.h"
#include "core/packet.h"
#include "core/types/chat.h"
#include "core/types/locationRead.h"
#include "core/types/message.h"
#include "core/types/messageRead.h"
#include "core/types/update.h"
#include "core/types/user.h"
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
}

class FastBot2 {
   public:
    // режим опроса обновлений
    enum class Poll : uint8_t {
        Sync,   // синхронный (рекомендуемый период > 3500 мс)
        Async,  // асинхронный (рекомендуемый период > 3500 мс)
        Long,   // асинхронный long polling
    };

    // разрешение и запрет типов обновлений
    fb::Updates updates;

    // передать клиента и запустить
    FastBot2(Client* client = nullptr) {
#if !defined(FB2_CUSTOM_CLIENT) && (defined(ESP8266) || defined(ESP32))
        setClient(&espclient);
        espclient.setInsecure();
        //client.setBufferSizes(512, 512);  // TODO
#endif
        if (client) setClient(client);
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

    // отправить сообщение. Для отправки подряд нескольких сообщений лучше поставить wait true - ждать ответ сервера
    void sendMessage(fb::Message& msg, bool wait = false) {
        if (!msg.text.length() || !msg.chat_id.valid()) return;

        fb::packet p(fbcmd::sendMessage(), _token);
        p.addStr(fbapi::chat_id(), msg.chat_id);
        p.addStr(fbapi::text(), msg.text);
        if (msg.thread_id >= 0) p.addInt(fbapi::message_thread_id(), msg.thread_id);
        if (msg.reply_to >= 0) p.addInt(fbapi::reply_to_message_id(), msg.reply_to);
        if (msg.disable_preview) p.addBool(fbapi::disable_web_page_preview(), true);
        if (msg.disable_notification) p.addBool(fbapi::disable_notification(), true);
        if (msg.protect) p.addBool(fbapi::protect_content(), true);
        if (msg.mode != fb::Message::Mode::None) p.addStr(fbapi::parse_mode(), msg.mode == (fb::Message::Mode::MarkdownV2) ? F("MarkdownV2") : F("HTML"));
        sendPacket(p, wait);
    }

    // ============================== CUSTOM ==============================

    // начать пакет для ручной отправки в API
    fb::packet beginPacket(const __FlashStringHelper* cmd) {
        return fb::packet(cmd, _token);
    }

    // отправить данные
    void sendPacket(fb::packet& p, bool wait = false) {
        _poll_wait = 0;
        if (wait && _allow_send_wait) {
            String s = _client.send_read(p, &_error);
            _parse(s);
        } else {
            _client.send(p);
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

    fb::CallbackUpdate _cbu1 = nullptr;
    fb::CallbackUpdate _cbu2 = nullptr;
    fb::CallbackResult _cbr = nullptr;

    void _getUpdates() {
        fb::packet p(fbcmd::getUpdates(), _token);
        if (_poll_mode == Poll::Long) p.addInt(fbapi::timeout(), _poll_tout / 1000);
        p.addInt(fbapi::limit(), _poll_limit);
        p.addInt(fbapi::offset(), _poll_offset);
        updates.fill(p);

        if (_poll_mode == Poll::Sync) {
            String s = _client.send_read(p, &_error);
            _parse(s);
        } else {
            _poll_wait = _client.send(p);
        }
    }

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

        gson::Doc doc(20);
        if (!doc.parse(s)) return _error = fb::Error::Parse;

        doc.hashKeys();
        if (!doc[fbhash::ok]) return _error = fb::Error::Telegram;

        gson::Entry result = doc[fbhash::result];
        if (!result.valid()) return _error = fb::Error::Telegram;

        // ============= UPDATES =============
        if (result.type() == gson::Type::Array) {
            uint8_t len = result.length();
            if (len) _poll_offset = result[0][fbhash::update_id].toInt32() + len;

            for (uint8_t i = 0; i < len; i++) {
                gson::Entry upd = result[i][1];
                if (!upd.valid()) continue;

                fb::Update update(upd, upd.keyHash());
                if (_cbu1) _cbu1(update);
                if (_cbu2) _cbu2(update);
                yield();
            }
        } else {
            // ============= INFO =============
            _allow_send_wait = 0;
            if (_cbr) _cbr(result);
            _allow_send_wait = 1;
        }

        return fb::Error::None;
    }
};