#pragma once
#include <Arduino.h>
#include <GSON.h>

namespace fb {
class BracketAccess {
   public:
    BracketAccess(const gson::Entry& entry) : entry(entry) {}

    // содержит элемент с указанным ключом
    // bool includes(const su::Text& key) const {
    //     return entry.get(key).valid();
    // }

    // доступ по ключу (контейнер - Object)
    // gson::Entry operator[](const char* key) const {
    //     return entry[key];
    // }
    // gson::Entry operator[](const __FlashStringHelper* key) const {
    //     return entry[key];
    // }
    // gson::Entry operator[](const String& key) const {
    //     return entry[key];
    // }
    // gson::Entry operator[](String& key) const {
    //     return entry[key];
    // }

    // содержит элемент с указанным хэшем ключа
    bool includes(size_t hash) const {
        return entry[hash].valid();
    }

    // доступ по хэшу ключа (контейнер - Object)
    gson::Entry operator[](size_t hash) const {
        return entry[hash];
    }

    // доступ по индексу (контейнер - Array или Object)
    gson::Entry operator[](int index) const {
        return entry[index];
    }

    const gson::Entry entry;
};

}  // namespace fb