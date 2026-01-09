#include <search/synonymous/StemmerPipeline.hpp>
#include <iostream>


void StemmerPipeline::changeLanguage(std::string_view language) {
    steammer.changeLanguage(language);
}

string StemmerPipeline::stem(string&& word) {
    if (word.size() <= 3)return word;
    // 1. Приводим к нижнему регистру (уже в UTF-16 через ICU)
    lower_case(word);

    // 2. Конвертируем UTF-16 -> UTF-8 для Snowball
    std::string utf8_word;
    icu::UnicodeString(reinterpret_cast<const UChar*>(word.data()), static_cast<int32_t>(word.size()))
        .toUTF8String(utf8_word);

    // 3. Стеммим (Snowball работает с UTF-8)
    std::string stemmed_utf8 = steammer.stem(std::move(utf8_word));

    // 4. Конвертируем результат обратно в UTF-16
    return string(icu::UnicodeString::fromUTF8(stemmed_utf8).getTerminatedBuffer());
}

const std::u16string& StemmerPipeline::stem_lowercased(std::u16string_view word_v) {
    utf8_buf.clear();
    icu::UnicodeString(word_v.data(), (int32_t)word_v.size()).toUTF8String(utf8_buf);

    std::string_view utf8_root = steammer.stem(utf8_buf);

    icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(utf8_root);
    utf16_buf.assign(ustr.getBuffer(), ustr.length());

    return utf16_buf;
}
