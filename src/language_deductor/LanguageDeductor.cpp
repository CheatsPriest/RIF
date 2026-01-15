#include "LanguageDeductor.hpp"
#include <array>
#include <algorithm>
#define binary_deductor1

struct language_interval {
	char_t left;
	char_t right;
	Languages language;
	constexpr language_interval(char_t left_, char_t right_, Languages language_) : left(left_), right(right_), language(language_) {};
	constexpr bool operator <(const language_interval& rhs) const {
		return left < rhs.left;
	}
	constexpr bool operator >(const language_interval& rhs) const {
		return left > rhs.left;
	}
	constexpr bool operator ==(const language_interval& rhs) const {
		return left == rhs.left;
	}
};

constexpr size_t raw_language_array_size = 6;

consteval auto create_sorted_language_array() {
	std::array<language_interval, raw_language_array_size> arr = { {
		{u'a', u'z', Languages::ENGLISH},
		{u'A', u'Z', Languages::ENGLISH},
		{u'а', u'я', Languages::RUSSIAN},
		{u'А', u'Я', Languages::RUSSIAN},
		{u'Ё', u'Ё', Languages::RUSSIAN},
		{u'ё', u'ё', Languages::RUSSIAN}
	} };

	std::sort(arr.begin(), arr.end());
	return arr;
}

static constexpr auto language_array = create_sorted_language_array();

constexpr Languages deductLanguageStatic(char_t symbol) noexcept {

#ifdef binary_deductor
	size_t left = 0;
	size_t right = language_array.size() - 1;

	while (left <= right) {
		size_t middle = left + (right - left) / 2;
		const auto& cur = language_array[middle];

		if (symbol < cur.left) {
			if (middle == 0) break;
			right = middle - 1;
		}
		else if (symbol > cur.right) {
			left = middle + 1;
		}
		else {
			return cur.language;
		}
	}
	return Languages::OTHER;
#else 
	for (size_t i = 0, n = language_array.size(); i < n; i++) {
		const auto& cur = language_array[i];
		if (cur.left <= symbol and symbol <= cur.right) {
			return cur.language;
		}
	}
	return Languages::OTHER;
#endif
}


static constexpr auto lang_lut = []() {
	std::array<Languages, 1280> lut{};
	for (char_t i = 0; i < 1280; ++i) {
		lut[i] = deductLanguageStatic(i);
	}
	return lut;
	}();

Languages deductLanguage(char_t symbol) noexcept {
	if (symbol < 1280) [[likely]] return lang_lut[symbol];
	return Languages::OTHER; 
}

bool isKnownSymbol(char_t symbol) noexcept
{
	return deductLanguage(symbol)!= Languages::OTHER;
}

static_assert(deductLanguageStatic(u'A') == Languages::ENGLISH);
static_assert(deductLanguageStatic(u'Z') == Languages::ENGLISH);
static_assert(deductLanguageStatic(u'a') == Languages::ENGLISH);
static_assert(deductLanguageStatic(u'z') == Languages::ENGLISH);

static_assert(deductLanguageStatic(u'А') == Languages::RUSSIAN);
static_assert(deductLanguageStatic(u'Я') == Languages::RUSSIAN);
static_assert(deductLanguageStatic(u'а') == Languages::RUSSIAN);
static_assert(deductLanguageStatic(u'я') == Languages::RUSSIAN);
static_assert(deductLanguageStatic(u'Ё') == Languages::RUSSIAN);
static_assert(deductLanguageStatic(u'ё') == Languages::RUSSIAN);

static_assert(deductLanguageStatic(u'@') == Languages::OTHER);
static_assert(deductLanguageStatic(u'[') == Languages::OTHER);
static_assert(deductLanguageStatic(u'`') == Languages::OTHER);
static_assert(deductLanguageStatic(u'{') == Languages::OTHER);
