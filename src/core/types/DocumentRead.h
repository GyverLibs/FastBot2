#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StringUtils.h>

#include "VirtualFastBot2_class.h"
#include "core/api.h"

namespace fb {

typedef void (*DownloadHook)(void* bot, const su::Text& id);

// https://core.telegram.org/bots/api#document
struct DocumentRead : public gson::Entry {
    DocumentRead(gson::Entry entry) : gson::Entry(entry) {}

    // id документа, можно использовать для скачивания
    su::Text id() {
        return (*this)[fbh::api::file_id];
    }

    // уникальный id документа в системе
    su::Text uniqueID() {
        return (*this)[fbh::api::file_unique_id];
    }

    // имя документа
    su::Text name() {
        return (*this)[fbh::api::file_name];
    }

    // MIME тип документа
    su::Text type() {
        return (*this)[fbh::api::mime_type];
    }

    // размер документа
    su::Text size() {
        return (*this)[fbh::api::file_size];
    }
};

}  // namespace fb