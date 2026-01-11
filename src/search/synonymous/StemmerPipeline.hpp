#pragma once
#include <search/synonymous/Stemmer.hpp>

class StemmerPipeline {
private:
    Stemmer steammer;
    Lowercaser lower_case;

    string utf8_buf;
public:
    StemmerPipeline(std::string_view language) : steammer(language) {

    }
    string stem(string&& word);
    string_view stem_lowercased(string_view word_v);
    void changeLanguage(std::string_view language);

    ~StemmerPipeline() = default;
};