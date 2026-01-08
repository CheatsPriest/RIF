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
    return icu::UnicodeString::fromUTF8(stemmed_utf8).getTerminatedBuffer();
}

string StemmerPipeline::stem(string_view word_v)
{
    return stem(string(word_v));
}
