#include <search/synonymous/Stemmer.hpp>


std::string Stemmer::stem(std::string&& word_cp1251) {
    if (!stemmer_ptr || word_cp1251.empty()) return word_cp1251;


    std::string word_utf8 = formatFromLocal(std::move(word_cp1251));


    const unsigned char* result = sb_stemmer_stem(
        stemmer_ptr.get(),
        reinterpret_cast<const unsigned char*>(word_utf8.c_str()),
        static_cast<int>(word_utf8.size())
    );

    if (result) {
        std::string result_utf8(reinterpret_cast<const char*>(result));
        return formatToLocal(std::move(result_utf8));
    }

    return word_utf8;
}

void Stemmer::changeLanguage(std::string_view language) {
    stemmer_ptr.reset(sb_stemmer_new(language.data(), "UTF_8"));
}
