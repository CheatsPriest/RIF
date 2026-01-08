#pragma once
#include <search/synonymous/Stemmer.hpp>

class StemmerPipeline {
private:
    Stemmer steammer;
    Lowercaser lower_case;
public:
    StemmerPipeline(std::string_view language) : steammer(language) {

    }
    string stem(string&& word);
    string stem(string_view word_v);
    void changeLanguage(std::string_view language);

    ~StemmerPipeline() = default;
};