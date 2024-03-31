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

   public:
    enum class Reboot : uint8_t {
        Idle,
        Triggered,
        WaitUpdate,
        CanReboot,
    };

    Fetcher(Reboot* reboot) : _reboot(reboot) {}

    ~Fetcher() {
        flush();
    }

    // очистить очередь на приём
    void flush() {
        while (_stream && _stream->available() && _stream->read() >= 0) yield();
    }

    // напечатать в принт
    template <typename T>
    bool writeTo(T& p) {
        if (!available()) return 0;

        uint8_t* buf = new uint8_t[FB_BLOCK_SIZE];
        if (buf) {
            while (available()) {
                size_t read = _stream->readBytes(buf, min(_stream->available(), FB_BLOCK_SIZE));
                if (read != p.write(buf, read)) {
                    FB_LOG("write error");
                    flush();
                    return 0;
                }
                yield();
            }
            delete[] buf;
        } else {
            FB_LOG("allocate error");
        }
        flush();
        return 1;
    }

    // обновить прошивку (ESP)
    bool updateFlash() {
        if (!available()) return 0;
        return _ota(true);
    }

    // обновить файловую систему (ESP)
    bool updateFS() {
        if (!available()) return 0;
        return _ota(false);
    }

    // есть данные для чтения
    int available() {
        return _stream ? _stream->available() : 0;
    }

    operator bool() {
        return available();
    }

   private:
    Stream* _stream = nullptr;
    Reboot* _reboot;

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
            flush();
            FB_LOG("ota begin error");
            return 0;
        }
        if (!writeTo(::Update)) {
            FB_LOG("ota write error");
            flush();
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