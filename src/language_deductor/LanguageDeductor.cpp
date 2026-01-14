#include "LanguageDeductor.hpp"
#include <array>
#include <algorithm>


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

Languages deductLanguage(char_t symbol) noexcept {
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
}

//static_assert(deductLanguage(u'A') == Languages::ENGLISH);
//static_assert(deductLanguage(u'Z') == Languages::ENGLISH);
//static_assert(deductLanguage(u'a') == Languages::ENGLISH);
//static_assert(deductLanguage(u'z') == Languages::ENGLISH);
//
//static_assert(deductLanguage(u'А') == Languages::RUSSIAN);
//static_assert(deductLanguage(u'Я') == Languages::RUSSIAN);
//static_assert(deductLanguage(u'а') == Languages::RUSSIAN);
//static_assert(deductLanguage(u'я') == Languages::RUSSIAN);
//static_assert(deductLanguage(u'Ё') == Languages::RUSSIAN);
//static_assert(deductLanguage(u'ё') == Languages::RUSSIAN);
//
//static_assert(deductLanguage(u'@') == Languages::OTHER);  
//static_assert(deductLanguage(u'[') == Languages::OTHER); 
//static_assert(deductLanguage(u'`') == Languages::OTHER);  
//static_assert(deductLanguage(u'{') == Languages::OTHER);  
