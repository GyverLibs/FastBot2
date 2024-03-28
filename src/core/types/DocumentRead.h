#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "core/api.h"

namespace fb {

// https://core.telegram.org/bots/api#document
struct DocumentRead {
    DocumentRead(gson::Entry entry) : entry(entry) {}

    // id документа
    su::Text id() {
        return entry[fbh::api::file_id];
    }

    // уникальный id документа
    su::Text uniqueID() {
        return entry[fbh::api::file_unique_id];
    }

    // имя документа
    su::Text name() {
        return entry[fbh::api::file_name];
    }

    // тип документа
    su::Text type() {
        return entry[fbh::api::mime_type];
    }

    // размер документа
    su::Text size() {
        return entry[fbh::api::file_size];
    }

    gson::Entry entry;
};

}  // namespace fb