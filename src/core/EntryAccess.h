#pragma once
#include <Arduino.h>
#include <GSON.h>

namespace fb {
class EntryAccess {
   public:
    EntryAccess(const gson::Entry& entry) : entry(entry) {}

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

    gson::Entry entry;
};

}  // namespace fb