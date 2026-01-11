#pragma once
#include <string>
#include <unicode/unistr.h>
#include <unicode/uchar.h>

#include <iostream>

using char_t =  char8_t;
using string = std::u8string;
using string_view = std::u8string_view;




class Lowercaser {
public:
    Lowercaser() = default;

    // 1. Для всей строки (самый частый случай)
    void operator()(string& str) const noexcept {
        if (str.empty()) return;

        // Создаем UnicodeString из UTF-8 данных
        icu::UnicodeString uStr = icu::UnicodeString::fromUTF8(
            reinterpret_cast<const char*>(str.data())
        );

        uStr.toLower(); 

        str.clear();
        std::string temp;
        uStr.toUTF8String(temp);

        // Перекладываем в наш string (u8string) через move
        str.assign(reinterpret_cast<const char_t*>(temp.data()), temp.size());
    }

    char_t operator()(char_t c) const noexcept {
        if (c <= 127) { // Быстрый путь для латиницы
            return static_cast<char_t>(std::tolower(static_cast<unsigned char>(c)));
        }
        return c;
    }

    string operator()(string&& str) const noexcept {
        operator()(str);
        return std::move(str);
    }
};

std::ostream& operator<<(std::ostream& os, const string& utf16_str);

std::ostream& operator<<(std::ostream& os, const char_t utf16_char);
