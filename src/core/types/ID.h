#pragma once
#include <Arduino.h>
#include <StringUtils.h>

namespace fb {

class ID {
   public:
    ID() {}
    ID(const Text& t) : _len(t.toStr(_str, 22)) {}
    ID(const String& s) : ID(Text(s)) {}
    ID(const long long& v) : _len(su::int64ToStr(v, _str)) {}
    ID(const char* str) {
        strncpy(_str, str, 22);
        _len = strlen(_str);
    }
    ID(const __FlashStringHelper* str) {
        strncpy_P(_str, (PGM_P)str, 22);
        _len = strlen_P((PGM_P)_str);
    }

    explicit operator bool() const {
        return _len;
    }

    operator Text() const {
        return Text(_str, _len);
    }

   private:
    char _str[22] = {};
    uint8_t _len = 0;
};

}  // namespace fb