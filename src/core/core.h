#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "Fetcher.h"
#include "api.h"
#include "bot_config.h"
#include "http.h"
#include "packet.h"
#include "result.h"
#include "types/Update.h"
#include "updates.h"

namespace fb {
// режим опроса обновлений
enum class Poll : uint8_t {
    Sync,   // синхронный (рекомендуемый период > 3500 мс)
    Async,  // асинхронный (рекомендуемый период > 3500 мс)
    Long,   // асинхронный long polling (рекомендуемый период > 20000 мс)
};

class Core : public Http {
    typedef void (*CallbackRaw)(Text response);
    typedef void (*CallbackResult)(gson::Entry& entry);
    typedef void (*CallbackUpdate)(Update& upd);

   public:
    // разрешение и запрет типов обновлений
    Updates updates;

    Core(Client& client) : Http(client) {
        _token.reserve(47);
    }

    // установить таймаут ожидания ответа сервера (умолч. 2000 мс)
    void setTimeout(uint16_t timeout) {
        _clientTout = timeout;
        http.setTimeout(timeout);
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

    // установить лимит памяти на ответ сервера (библиотека начнёт пропускать сообщения), умолч. 20000
    void setMemLimit(size_t limit) {
        _limit = limit;
    }

    // установить лимит - кол-во сообщений в одном обновлении (умолч. 3)
    void setLimit(uint8_t limit = 3) {
        _poll_limit = limit ? limit : 1;
    }

    // установить режим и период опроса (умолч. Poll::Sync и 4000 мс)
    void setPollMode(Poll mode = Poll::Sync, uint16_t period = 4000) {
        _poll_mode = mode;
        _poll_prd = period;
    }

    // получить режим опроса
    Poll getPollMode() {
        return _poll_mode;
    }

    // пропустить непрочитанные сообщения, отрицательное число, соответствует количеству пропусков. Вызывать однократно
    //  https://core.telegram.org/bots/api#getupdates
    void skipUpdates(int32_t offset = -1) {
        _poll_offset = offset;
    }

    // пропустить следующее сообщение (сдвинуть оффсет на 1)
    void skipNextMessage() {
        _poll_offset++;
    }

    // id последнего отправленного сообщения от бота
    uint32_t lastBotMessage() {
        return _last_bot;
    }

    // запустить (по умолчанию уже запущен)
    void begin() {
        _state = true;
    }

    // остановить
    void end() {
        _state = false;
        http.stop();
    }

    // указать наличие подключения к Интернет (true). При отключенном апдейты не будут запрашиваться
    void setOnline(bool online) {
        _online = online;
    }

    // ============================== ATTACH ==============================

    // подключить обработчик обновлений вида void cb(Update& u) {}
    void attachUpdate(CallbackUpdate callback) {
        _cbUpdate = callback;
    }

    // отключить обработчик обновлений
    void detachUpdate() {
        _cbUpdate = nullptr;
    }

    // подключить обработчик результата вида void cb(gson::Entry& r) {}
    void attachResult(CallbackResult callback) {
        _cbResult = callback;
    }

    // отключить обработчик результата
    void detachResult() {
        _cbResult = nullptr;
    }

    // подключить обработчик сырого ответа сервера void cb(Text response) {}
    void attachRaw(CallbackRaw callback) {
        _cbRaw = callback;
    }

    // отключить обработчик ответа сервера
    void detachRaw() {
        _cbRaw = nullptr;
    }
    // ============================== TICK ==============================

    // тикер, вызывать в loop. Вернёт true, если был обработан Update
    bool tick() {
        if (!_state) return 0;

        if (http.isWaiting()) {
            if (millis() - _last_send >= (_clientTout + (_poll_wait ? _poll_prd : 0ul))) {
                _poll_wait = 0;
                FB_LOG("poll timeout");
                http.stop();
            }
        } else if (_online) {
            bool tout = millis() - _last_send >= (_poll_mode == Poll::Long ? FB_LONG_POLL_TOUT : _poll_prd);
            if (!_last_send || tout) {
                getUpdates(false);
            }
        }

        if (http.available()) {
            _poll_wait = 0;
            Result res = _parseResponse(http.getResponse());
            if (res && res.isArray()) {
                _parseUpdates(res);
                return 1;
            }
        }
        return 0;
    }

    // проверить обновления вручную (принудительно синхронно), вызовется обработчик
    bool tickManual() {
        Result res = getUpdates(true, false);
        if (res && res.isArray()) {
            _parseUpdates(res);
            return 1;
        }
        return 0;
    }

    // система ждёт ответа с обновлениями
    bool isPolling() {
        return _poll_wait;
    }

    // отправить запрос на обновление
    Result getUpdates(bool wait, bool allowLongPool = true) {
        FB_LOG("getUpdates");
        Packet p(tg_cmd::getUpdates, _token);
        if (_poll_mode == Poll::Long && allowLongPool) p[tg_api::timeout] = (uint16_t)(_poll_prd / 1000);
        p[tg_api::limit] = _poll_limit;
        p[tg_api::offset] = _poll_offset;
        updates.makePacket(p);

        if (_poll_mode == Poll::Sync || wait) {
            _poll_wait = 0;
            return sendPacket(p, true);
        } else {
            return sendPacket(p, false, &_poll_wait);
        }
    }

    // запросить перезагрузку устройства
    void reboot() {
        _reboot = Fetcher::Reboot::Triggered;
    }

    // можно перезагрузить устройство
    bool canReboot() {
        return _reboot == Fetcher::Reboot::CanReboot;
    }

    // ============================== MANUAL ==============================

    // отправить команду вручную
    Result sendCommand(const __FlashStringHelper* cmd, const String& json, bool wait = true) {
        Packet p(cmd, _token, json);
        return sendPacket(p, wait);
    }

    // отправить команду вручную без параметров
    Result sendCommand(const __FlashStringHelper* cmd, bool wait = true) {
        Packet p(cmd, _token, "");
        return sendPacket(p, wait);
    }

    // начать пакет для ручной отправки в API
    Packet beginPacket(const __FlashStringHelper* cmd) {
        return Packet(cmd, _token);
    }

    // отправить пакет
    Result sendPacket(Packet& packet, bool wait = true, bool* sent = nullptr) {
        if (_poll_wait) {
            _poll_wait = 0;
            FB_LOG("stop polling");
            http.stop();
        }

        FB_ESP_YIELD();
        if (http.beginSend()) {
            if (sent) *sent = true;
            packet.printTo(http);
            _last_send = millis();
            FB_ESP_YIELD();
            if (wait) {
                FB_LOG("send + wait");
                return _parseResponse(http.getResponse());
            } else {
                FB_LOG("send async");
            }
        } else {
            if (sent) *sent = false;
            FB_LOG("send error");
        }
        _last_send = millis();  // на случай долгого beginSend
        return Result();
    }

   protected:
    String _token;
    uint16_t _clientTout = 3000;
    uint16_t _poll_prd = 4000;
    Poll _poll_mode = Poll::Sync;
    uint8_t _poll_limit = 3;
    bool _poll_wait = 0;
    bool _query_answ = 0;
    bool _state = true;
    bool _online = true;
    int32_t _poll_offset = 0;
    uint32_t _last_bot = 0;
    uint32_t _last_send = 0;
    size_t _limit = 20000;
    Fetcher::Reboot _reboot = Fetcher::Reboot::Idle;

    CallbackUpdate _cbUpdate = nullptr;
    CallbackResult _cbResult = nullptr;
    CallbackRaw _cbRaw = nullptr;

    Result _parseResponse(ghttp::Client::Response resp) {
        FB_ESP_YIELD();
        if (resp) {
            if (resp.type() == F("application/json")) {
                Result res(resp.body());
                res.parseJson();
                http.flush();
                if (_cbRaw) _cbRaw(res.getRaw());
                FB_ESP_YIELD();
                if (res && res.isObject()) _parseResult(res);
                return res;
            } else if (resp.type() == F("application/octet-stream")) {
                return Result(resp.body());
            } else {
                FB_LOG("unknown response");
                http.flush();
            }
        } else {
            FB_LOG("bad response");
        }
        return Result();
    }

    void _parseResult(gson::Entry& result) {
        FB_LOG("got result");
        if (result.has(tg_apih::message_id)) _last_bot = result[tg_apih::message_id];
        if (_cbResult) _cbResult(result);
    }

    void _parseUpdates(gson::Entry& result) {
        if (_reboot == Fetcher::Reboot::WaitUpdate) {
            _reboot = Fetcher::Reboot::CanReboot;
#ifdef FB_ESP_BUILD
            ESP.restart();
#endif
            return;
        }
        uint8_t len = result.length();
        if (len) _poll_offset = result[0][tg_apih::update_id].toInt32();

        for (uint8_t i = 0; i < len; i++) {
            FB_ESP_YIELD();
            _poll_offset++;
            gson::Entry upd = result[i][1];
            if (!upd) continue;

            size_t typeHash = upd.keyHash();
            Update update(upd, typeHash);
            if (typeHash == tg_apih::callback_query) _query_answ = 0;

            if (_cbUpdate) _cbUpdate(update);
            FB_ESP_YIELD();

            if (typeHash == tg_apih::callback_query && !_query_answ) {
                _query_answ = true;
                fb::Packet p(tg_cmd::answerCallbackQuery, _token);
                p[tg_api::callback_query_id] = update.query().id();
                sendPacket(p, false);
            }

            if (_reboot == Fetcher::Reboot::Triggered) {
                _reboot = Fetcher::Reboot::WaitUpdate;
                return;
            }
        }
    }
};

}  // namespace fb