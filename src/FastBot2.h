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
    FastBot2(Client* client = nullptr) : _client(client) {
        begin();
        _token.reserve(47);
    }

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

    // установить лимит длины сообщения от сервера (умолч. 15000)
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

    // подключить обработчик вида void cb(fb::Update& u) {}
    void attach(fb::Callback callback) {
        _cb = callback;
    }

    // отключить обработчик
    void detach() {
        _cb = nullptr;
    }

    // подключить второй обработчик вида void cb(fb::Update& u) {}
    void attach2(fb::Callback callback) {
        _cb2 = callback;
    }

    // отключить второй обработчик
    void detach2() {
        _cb2 = nullptr;
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

    // прочитать ошибку
    fb::Error getError() {
        fb::Error buf = _error;
        _error = fb::Error::None;
        return buf;
    }

    // ============================== SEND ==============================

    // отправить сообщение
    void sendMessage(fb::Message& msg) {
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
        send(p);
    }

    // ============================== CUSTOM ==============================

    fb::packet makePacket(const __FlashStringHelper* cmd) {
        return fb::packet(cmd, _token);
    }

    // отправить данные
    void send(fb::packet& p) {
        _poll_wait = 0;
        _client.send(p);
    }

    // подключен ли клиент
    bool _connected() {
        return _client.connected();
    }

    // ===================== PRIVATE =====================
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

    fb::Callback _cb = nullptr;
    fb::Callback _cb2 = nullptr;

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

        // ============= UPDATES =============
        if (result.type() == gson::Type::Array) {
            uint8_t len = result.length();

            for (uint8_t i = 0; i < len; i++) {
                if (i == 0) _poll_offset = result[0][fbhash::update_id].toInt32() + len;
                gson::Entry upd = result[i][1];
                if (!upd.valid()) continue;

                fb::Update update(upd, upd.keyHash());
                if (_cb) _cb(update);
                if (_cb2) _cb2(update);
                yield();
            }

        } else {
            // ============= INFO =============
        }

        return fb::Error::None;
    }
};