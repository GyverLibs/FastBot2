#pragma once

#define TELEGRAM_HOST "api.telegram.org"  // хост
#define TELEGRAM_PORT 443                 // ssl порт
#define FB_BLOCK_SIZE 256                 // буфер файлов
#define FB_LONG_POLL_TOUT 300             // пауза между long poll update

#ifdef ESP8266
#define FB_ESP_YIELD() delay(0);  // esp_yield();//optimistic_yield(2000);
#else
#define FB_ESP_YIELD()
#endif

// FB_NO_OTA
// FB_NO_FILE

#if defined(ESP8266) || defined(ESP32)
#define FB_ESP_BUILD 1
#endif

// #define FB_USE_LOG Serial

// log
#ifdef FB_USE_LOG
#define FB_LOG(x)                                   \
    do {                                            \
        FB_USE_LOG.print(F("> "));                  \
        FB_USE_LOG.print(x);                        \
        FB_USE_LOG.print(F(" in "));                \
        FB_USE_LOG.print(__FUNCTION__);             \
        FB_USE_LOG.print(F("() [" __FILE__ " : ")); \
        FB_USE_LOG.print(__LINE__);                 \
        FB_USE_LOG.println(']');                    \
    } while (0);
#else
#define FB_LOG(x)
#endif