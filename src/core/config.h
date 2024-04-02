#pragma once

#define TELEGRAM_HOST "api.telegram.org"  // хост
#define TELEGRAM_PORT 443                 // ssl порт
#define FB_BLOCK_SIZE 512                 // буфер файлов
#define FB_LONG_POLL_TOUT 300             // пауза между long poll update

// FB_NO_OTA
// FB_NO_FILE

#if defined(ESP8266) || defined(ESP32)
#define FB_ESP_BUILD 1
#endif

#define FB_USE_LOG Serial

#ifdef FB_USE_LOG
/*
static char* __stack_start = nullptr;
#define FB_LOG(x)                                               \
    do {                                                        \
        if (!__stack_start) {                                   \
            char t;                                             \
            __stack_start = &t;                                 \
        }                                                       \
        FB_USE_LOG.print(F("> "));                              \
        FB_USE_LOG.print(x);                                    \
        FB_USE_LOG.print(F(" in "));                            \
        FB_USE_LOG.print(__FUNCTION__);                         \
        FB_USE_LOG.print(F("() [" __FILE__ " : "));             \
        FB_USE_LOG.print(__LINE__);                             \
        FB_USE_LOG.print(']');                                  \
        FB_USE_LOG.print(F(", stack: "));                       \
        char t;                                                 \
        FB_USE_LOG.print((size_t)__stack_start - (size_t) & t); \
        FB_USE_LOG.println();                                   \
    } while (0);
*/
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