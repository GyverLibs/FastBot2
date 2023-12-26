#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/keys.h"

namespace fb {

class Message {
   public:
    enum class Mode : uint8_t {
        None,
        MarkdownV2,
        HTML,
    };

    // текст сообщения
    String text;

    // id чата, куда отправлять
    sutil::AnyValue chat_id;

    // id темы в группе
    int32_t thread_id = -1;

    // ответить на сообщение с указанным id
    int32_t reply_to = -1;

    // отключить превью для ссылок
    bool disable_preview = 0;

    // отправить без уведомления
    bool disable_notification = 0;

    // защитить от пересылки и копирования
    bool protect = 0;

    // режим текста MarkdownV2, HTML
    Mode mode = Mode::None;

    // TODO reply markup

   private:
};

}