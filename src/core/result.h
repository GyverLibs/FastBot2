#pragma once
#include <Arduino.h>
#include <GSON.h>
#include <StreamReader.h>

#include "api.h"
#include "core_class.h"

namespace fb {

class Result : public gson::Entry {
    friend class fb::Core;

   public:
    Result() {}
    Result(StreamReader reader) : _reader(reader) {}

    enum class Type : uint8_t {
        Empty,
        OK,
        Error,
    };

    // освободить память
    void reset() {
        _parser.reset();
        gson::Entry::reset();
    }

    // получить ридер
    StreamReader& getReader() {
        return _reader;
    }

    // получить скачанный json пакет как Text
    Text getRaw() {
        return _parser.getRaw();
    }

    // тип результата
    Type type() {
        if (_parser.has(tg_apih::result)) return Type::OK;
        if (_parser[tg_apih::ok] == false) return Type::Error;
        return Type::Empty;
    }

    // результат - ошибка
    bool isError() {
        return _parser[tg_apih::ok] == false;
    }

    // результат пуст
    bool isEmpty() {
        return !_parser.length();
    }

    // получить текст ошибки
    Text getError() {
        return _parser[tg_apih::description];
    }

    // получить код ошибки
    Text getErrorCode() {
        return _parser[H(error_code)];
    }

    Result(Result& res) {
        move(res);
    }
    Result(Result&& res) noexcept {
        move(res);
    }
    Result& operator=(Result& res) {
        move(res);
        return *this;
    }
    Result& operator=(Result&& res) noexcept {
        move(res);
        return *this;
    }

    void move(Result& res) noexcept {
        if (this == &res) return;
        _parser = gtl::move(res._parser);
        *((gson::Entry*)this) = _parser[tg_apih::result];
        _reader = res._reader;
    }

    gson::ParserStream _parser;

   private:
    StreamReader _reader;

    void parseJson() {
        if (_reader && _parser.parse(_reader.stream, _reader.length())) {
            _parser.hashKeys();
            *((gson::Entry*)this) = _parser[tg_apih::result];
        }
    }
};

}  // namespace fb