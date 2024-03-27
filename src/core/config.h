#pragma once

#define TELEGRAM_HOST "api.telegram.org"  // хост
#define TELEGRAM_PORT 443                 // ssl порт
#define FB_BLOCK_SIZE 1024                // буфер отправки файла
#define FB_LONG_POLL_TOUT 300             // пауза между long poll update

#if defined(ESP8266) || defined(ESP32)
#define FB_ESP_BUILD 1
#endif