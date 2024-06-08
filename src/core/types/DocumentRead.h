#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "FastBot2Client_class.h"
#include "core/EntryAccess.h"
#include "core/api.h"

namespace fb {

typedef void (*DownloadHook)(void* bot, const su::Text& id);

// https://core.telegram.org/bots/api#document
struct DocumentRead : public EntryAccess {
    DocumentRead(gson::Entry entry) : EntryAccess(entry) {}

    // id документа, можно использовать для скачивания
    su::Text id() {
        return entry[tg_apih::file_id];
    }

    // уникальный id документа в системе
    su::Text uniqueID() {
        return entry[tg_apih::file_unique_id];
    }

    // имя документа
    su::Text name() {
        return entry[tg_apih::file_name];
    }

    // MIME тип документа
    su::Text type() {
        return entry[tg_apih::mime_type];
    }

    // размер документа
    su::Text size() {
        return entry[tg_apih::file_size];
    }
};

}  // namespace fb