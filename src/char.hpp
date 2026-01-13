#pragma once
#include <string>
#include <unicode/unistr.h>
#include <unicode/uchar.h>

#include <iostream>

using char_t =  char16_t;
using string = std::u16string;
using string_view = std::u16string_view;




class Lowercaser {
public:
    Lowercaser() = default;

    void operator()(string& str) const noexcept {
        if (str.empty()) return;

        icu::UnicodeString uStr(reinterpret_cast<const UChar*>(str.data()),
            static_cast<int32_t>(str.size()));
        uStr.toLower();


        if (uStr.length() != static_cast<int32_t>(str.size())) {
            str.resize(uStr.length());
        }

        uStr.extract(0, uStr.length(), reinterpret_cast<UChar*>(str.data()));
    }

    
    char_t operator()(char_t c) const noexcept {
        return static_cast<char_t>(u_tolower(static_cast<UChar32>(c)));
    }
    string operator()(string&& str) const noexcept {
        operator()(str);
        return std::move(str);
    }

    void operator()(std::string& utf8_str) const noexcept {
        if (utf8_str.empty()) return;

        icu::UnicodeString uStr = icu::UnicodeString::fromUTF8(utf8_str.c_str());

        uStr.toLower();

        std::string result;
        uStr.toUTF8String(result);

        utf8_str = std::move(result);
    }
};

std::ostream& operator<<(std::ostream& os, const std::u16string& utf16_str);

std::ostream& operator<<(std::ostream& os, const char16_t utf16_char);
