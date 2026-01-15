#pragma once


#include <char.hpp>
#include <array>

enum class Languages : uint8_t
{
	RUSSIAN = 0,
	ENGLISH = 1,
	OTHER
};

constexpr static size_t amount_of_languages = (size_t)Languages::OTHER;

constexpr static auto list_of_languages = std::to_array({ "russian", "english" });
 

Languages deductLanguage(char_t symbol) noexcept;

bool isKnownSymbol(char_t symbol) noexcept;