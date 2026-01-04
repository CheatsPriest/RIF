#pragma once
#include <search/synonymous/Stemmer.hpp>

class StemmerPipeline {
private:
    Stemmer steammer;
    Lowercaser lower_case;
public:
    StemmerPipeline(std::string_view language) : steammer(language) {

    }
    std::string stem(std::string&& word);

    void changeLanguage(std::string_view language);

    ~StemmerPipeline() = default;
};