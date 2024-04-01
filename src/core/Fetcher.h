#pragma once
#include <Arduino.h>

#include "VirtualFastBot2_class.h"
#include "config.h"

#if defined(FB_ESP_BUILD) && !defined(FB_NO_OTA)
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <Updater.h>
#include <flash_hal.h>
#else  // ESP32
#include <Update.h>
#include <WiFi.h>
#endif
#endif  // FB_ESP_BUILD

namespace fb {

class Fetcher {
    friend class ::VirtualFastBot2;

    enum class Reboot : uint8_t {
        Idle,
        Triggered,
        WaitUpdate,
        CanReboot,
    };

   public:
    Fetcher(Reboot* reboot) : _reboot(reboot) {}

    ~Fetcher() {
        _flush();
    }

    // напечатать в принт
    template <typename T>
    bool writeTo(T& p) {
        if (!available()) return 0;

        uint8_t* buf = new uint8_t[FB_BLOCK_SIZE];
        delay(1);  // TODO
        if (buf) {
            delay(1);  // TODO
            while (stream->available()) {
                size_t read = stream->readBytes(buf, min(stream->available(), FB_BLOCK_SIZE));
                delay(1);  // TODO
                if (read != p.write(buf, read)) {
                    FB_LOG("write error");
                    _flush();
                    return 0;
                }
                delay(1);  // TODO
            }
            delete[] buf;
        } else {
            FB_LOG("allocate error");
        }
        _flush();
        return 1;
    }

    // обновить прошивку (ESP)
    bool updateFlash() {
        return available() ? _ota(true) : 0;
    }

    // обновить файловую систему (ESP)
    bool updateFS() {
        return available() ? _ota(false) : 0;
    }

    // есть данные для чтения
    int available() {
        return stream ? stream->available() : 0;
    }

    operator bool() {
        return available();
    }

    Stream* stream = nullptr;

   private:
    Reboot* _reboot;

    // очистить очередь на приём
    void _flush() {
        while (stream && stream->available()) {
            stream->read();
            yield();
        }
    }

    bool _ota(bool ota_flash = true) {
#if defined(FB_ESP_BUILD) && !defined(FB_NO_OTA)
        size_t ota_size = 0;
        int ota_type = 0;

        if (ota_flash) {
            ota_type = U_FLASH;
#ifdef ESP8266
            ota_size = (size_t)((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000);
#else
            ota_size = UPDATE_SIZE_UNKNOWN;
#endif
        } else {
#ifdef ESP8266
            ota_type = U_FS;
            close_all_fs();
            ota_size = (size_t)&_FS_end - (size_t)&_FS_start;
#else
            ota_type = U_SPIFFS;
            ota_size = UPDATE_SIZE_UNKNOWN;
#endif
        }
        if (!::Update.begin(ota_size, ota_type)) {
            _flush();
            FB_LOG("ota begin error");
            return 0;
        }
        if (!writeTo(::Update)) {
            FB_LOG("ota write error");
            _flush();
            return 0;
        }
        if (::Update.end(true)) {
            *_reboot = Reboot::Triggered;
            return 1;
        } else {
            FB_LOG("ota end error");
            return 0;
        }
#endif
    }
};

}  // namespace fb