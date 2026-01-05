#include <search/synonymous/StemmerPipeline.hpp>
#include <iostream>


void StemmerPipeline::changeLanguage(std::string_view language) {
    steammer.changeLanguage(language);
}

std::string StemmerPipeline::stem(std::string&& word) {
    lower_case(word);
    return steammer.stem(std::move(word));
}
