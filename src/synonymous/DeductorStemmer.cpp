#include <synonymous/DeductorStemmer.hpp>

StemmerPipeline& DeductorStemmer::whoWillWork(char_t first_symbol)
{
	return stemmer_per_language[(size_t)deductLanguage(first_symbol)];
}

string DeductorStemmer::stem(string&& word) {
	return whoWillWork(word.front()).stem(std::move(word));
}

const std::u16string& DeductorStemmer::stem_lowercased(std::u16string_view word_v)
{
	return whoWillWork(word_v.front()).stem_lowercased(word_v);
}
