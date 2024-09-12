#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "FastBot2Client_class.h"
#include "../EntryAccess.h"
#include "../api.h"

namespace fb {

typedef void (*DownloadHook)(void* bot, const Text& id);

// https://core.telegram.org/bots/api#document
struct DocumentRead : public EntryAccess {
    DocumentRead(gson::Entry entry) : EntryAccess(entry) {}

    // id документа, можно использовать для скачивания
    Text id() {
        return entry[tg_apih::file_id];
    }

    // уникальный id документа в системе
    Text uniqueID() {
        return entry[tg_apih::file_unique_id];
    }

    // имя документа
    Text name() {
        return entry[tg_apih::file_name];
    }

    // MIME тип документа
    Text type() {
        return entry[tg_apih::mime_type];
    }

    // размер документа
    Text size() {
        return entry[tg_apih::file_size];
    }
};

}  // namespace fb