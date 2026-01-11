#include <char.hpp>



std::ostream& operator<<(std::ostream& os, const string& utf16_str) {
    std::string utf8;
    icu::UnicodeString(reinterpret_cast<const UChar*>(utf16_str.data()),
        static_cast<int32_t>(utf16_str.size())).toUTF8String(utf8);
    return os << utf8;
}

std::ostream& operator<<(std::ostream& os, const char_t utf16_char) {
    std::u16string utf16_str{ utf16_char };
    std::string utf8;
    icu::UnicodeString(reinterpret_cast<const UChar*>(utf16_str.data()),
        static_cast<int32_t>(utf16_str.size())).toUTF8String(utf8);
    return os << utf8;
}
