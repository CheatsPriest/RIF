#pragma once
#include <string>


// UTF-8 → UTF-16
std::u16string utf8_to_utf16_icu(const std::string& utf8_str);

// UTF-16 → UTF-8
std::string utf16_to_utf8_icu(const std::u16string& utf16_str);

// Normalize
std::string normalizeU8ToStd(const std::u8string& input);