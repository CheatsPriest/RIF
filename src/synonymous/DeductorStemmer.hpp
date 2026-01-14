#pragma once

#include <language_deductor/LanguageDeductor.hpp>
#include <synonymous/StemmerPipeline.hpp>
#include <string>

class DeductorStemmer {
private:
	std::array< StemmerPipeline, amount_of_languages+1> stemmer_per_language;
	StemmerPipeline& whoWillWork(char_t first_symbol);
public:
	DeductorStemmer() {
		size_t i = 0;
		for (const auto& language : list_of_languages) {
			stemmer_per_language[i].changeLanguage(language);
			i++;
		}
	}
	string stem(string&& word);
	const std::u16string& stem_lowercased(std::u16string_view word_v);
};