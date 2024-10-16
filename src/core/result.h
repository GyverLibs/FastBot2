#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StreamReader.h>

#include "core_class.h"

namespace fb {

class Result : public gson::Entry {
    friend class fb::Core;

   public:
    Result() {}
    Result(StreamReader reader) : _reader(reader) {}

    // освободить память
    void reset() {
        _parser.reset();
    }

    // получить ридер
    StreamReader& getReader() {
        return _reader;
    }

    // получить скачанный json пакет как Text
    Text getRaw() {
        return _parser.getRaw();
    }

    Result(Result& res) {
        move(res);
    }
    Result& operator=(Result& res) {
        move(res);
        return *this;
    }

#if __cplusplus >= 201103L
    Result(Result&& res) noexcept {
        move(res);
    }
    Result& operator=(Result&& res) noexcept {
        move(res);
        return *this;
    }
#endif

    void move(Result& res) noexcept {
        if (this == &res) return;
        if (res) {
            _parser.move(res._parser);
            *((gson::Entry*)this) = _parser.getByIndex(res.index());
        }
        _reader = res._reader;
    }

    gson::ParserStream _parser;

   private:
    StreamReader _reader;

    bool parseJson() {
        if (!_reader) return 0;
        if (!_parser.parse(_reader.stream, _reader.length())) return 0;

        _parser.hashKeys();
        if (!_parser[tg_apih::ok].toBool()) return 0;

        gson::Entry result = _parser[tg_apih::result];
        if (!result) return 0;

        *((gson::Entry*)this) = result;
        return 1;
    }
};

}  // namespace fb