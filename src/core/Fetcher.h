#pragma once
#include <Arduino.h>
#include <GyverHTTP.h>

#include "../bot_config.h"
#include "core_class.h"

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

class Fetcher : public StreamReader {
    friend class Core;

    enum class Reboot : uint8_t {
        Idle,
        Triggered,
        WaitUpdate,
        CanReboot,
    };

   public:
    Fetcher(Reboot* reboot, StreamReader& reader) : StreamReader(reader), _reboot(reboot) {}

#if defined(FB_ESP_BUILD) && !defined(FB_NO_OTA)
    // обновить прошивку (ESP)
    bool updateFlash() {
        return length() ? _ota(true) : 0;
    }

    // обновить файловую систему (ESP)
    bool updateFS() {
        return length() ? _ota(false) : 0;
    }
#endif

   private:
    Reboot* _reboot;

#if defined(FB_ESP_BUILD) && !defined(FB_NO_OTA)
    bool _ota(bool ota_flash = true) {
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
            FB_LOG("ota begin error");
            return 0;
        }
        if (!writeTo(::Update)) {
            FB_LOG("ota write error");
            return 0;
        }
        if (::Update.end(true)) {
            *_reboot = Reboot::Triggered;
            return 1;
        } else {
            FB_LOG("ota end error");
            return 0;
        }
    }
#endif
};

}  // namespace fb