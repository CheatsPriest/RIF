#include <ICU/Decoders.hpp>
#include <unicode/unistr.h>
#include <unicode/normalizer2.h>
#include <unicode/utypes.h>

// UTF-8 → UTF-16
std::u16string utf8_to_utf16_icu(const std::string& utf8_str) {
    icu::UnicodeString unicode_str = icu::UnicodeString::fromUTF8(utf8_str);
    std::u16string utf16_str;

    int32_t length = unicode_str.length();
    utf16_str.resize(length);

    for (int32_t i = 0; i < length; ++i) {
        utf16_str[i] = (char16_t)unicode_str[i];
    }

    return utf16_str;
}

// UTF-16 → UTF-8
std::string utf16_to_utf8_icu(const std::u16string& utf16_str) {
    // Создаем UnicodeString из UTF-16 данных
    icu::UnicodeString unicode_str(
        reinterpret_cast<const UChar*>(utf16_str.data()),
        static_cast<int32_t>(utf16_str.length())
    );

    // Конвертируем в UTF-8 строку
    std::string utf8_str;
    unicode_str.toUTF8String(utf8_str);

    return utf8_str;
}

std::string normalizeU8ToStd(const std::u8string& input)
{
    UErrorCode status = U_ZERO_ERROR;

    // 1. Преобразуем u8string в UnicodeString (UTF-16 внутри ICU)
    // Используем reinterpret_cast, так как char8_t и char бинарно совместимы
    icu::UnicodeString uSource = icu::UnicodeString::fromUTF8(
        icu::StringPiece(reinterpret_cast<const char*>(input.data()), (int32_t)input.length())
    );

    const icu::Normalizer2* normalizer = icu::Normalizer2::getNFCInstance(status);
    if (U_FAILURE(status)) return std::string(reinterpret_cast<const char*>(input.data()), input.length());

    // 2. Нормализация
    icu::UnicodeString uResult;
    normalizer->normalize(uSource, uResult, status);
    if (U_FAILURE(status)) return std::string(reinterpret_cast<const char*>(input.data()), input.length());

    // 3. Выгружаем результат сразу в std::string
    std::string result;
    uResult.toUTF8String(result);

    return result;
}

