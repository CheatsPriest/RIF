#pragma once
#include <synonymous/Stemmer.hpp>

class StemmerPipeline {
private:
    Stemmer steammer;
    Lowercaser lower_case;

    std::string utf8_buf;       
    std::u16string utf16_buf;
public:
    StemmerPipeline(std::string_view language = "russian") : steammer(language) {

    }
    string stem(string&& word);
    const std::u16string& stem_lowercased(std::u16string_view word_v);
    void changeLanguage(std::string_view language);

    ~StemmerPipeline() = default;
};