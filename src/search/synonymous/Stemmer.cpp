#include <search/synonymous/Stemmer.hpp>


string Stemmer::stem(string&& word_utf8) {
    if (!stemmer_ptr || word_utf8.empty()) return word_utf8;

    const unsigned char* result = sb_stemmer_stem(
        stemmer_ptr.get(),
        reinterpret_cast<const unsigned char*>(word_utf8.c_str()),
        static_cast<int>(word_utf8.size())
    );

    if (result) {
        string result_utf8(reinterpret_cast<const char_t*>(result));
        return (std::move(result_utf8));
    }

    return word_utf8;
}

string_view Stemmer::stem(string_view word_v) {
    if (!stemmer_ptr || word_v.empty()) return word_v;

    const unsigned char* result = sb_stemmer_stem(
        stemmer_ptr.get(),
        reinterpret_cast<const unsigned char*>(word_v.data()),
        static_cast<int>(word_v.size())
    );

    if (result) {
        int len = sb_stemmer_length(stemmer_ptr.get());
        return string_view(reinterpret_cast<const char_t*>(result), len);
    }

    return word_v;
}


void Stemmer::changeLanguage(std::string_view language) {
    stemmer_ptr.reset(sb_stemmer_new(language.data(), "UTF_8"));
}
