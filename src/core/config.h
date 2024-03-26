#pragma once

#define TELEGRAM_HOST "api.telegram.org"
#define TELEGRAM_PORT 443
#define FB_BOUNDARY "---------FAST_BOT2"
#define FB_BOUNDARY_LEN 18
#define FB_BLOCK_SIZE 1024

#if defined(ESP8266) || defined(ESP32)
#define FB_ESP_BUILD 1
#endif