#pragma once
#include <Arduino.h>
#include <GSON.h>

namespace fb {
class EntryAccess {
   public:
    EntryAccess(const gson::Entry& entry) : entry(entry) {}

    // содержит элемент с указанным хэшем ключа
    bool has(size_t hash) const {
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

    // содержит элемент с указанным хэшем ключа
    bool includes(size_t hash) const __attribute__((deprecated)) {
        return has(hash);
    }
};

}  // namespace fb