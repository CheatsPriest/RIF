#include <search/synonymous/StemmerPipeline.hpp>
#include <iostream>


void StemmerPipeline::changeLanguage(std::string_view language) {
    steammer.changeLanguage(language);
}

string StemmerPipeline::stem(string&& word) {
    if (word.size() <= 3)return word;
    lower_case(word);
    return steammer.stem(std::move(word));
}

string_view StemmerPipeline::stem_lowercased(string_view word_v) {
    return steammer.stem(utf8_buf);;
}
