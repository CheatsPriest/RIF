// SearchConfig.hpp
#pragma once
#include <atomic>
#include <vector>
#include <string>
#include <unordered_set>

#include <char.hpp>

constexpr static unsigned start_amount_of_search_threads = 1;

class SearchConfig {
public:
	
	unsigned amount_of_search_threads = start_amount_of_search_threads;

	unsigned int depth = 5;
	bool respect_registers = false;
	bool full_words = false;


	std::unordered_set<std::string> allowed_extensions = { ".txt", ".md", ".cpp", ".c", ".hpp", ".h", ".docx"};

	std::vector<std::string> initial_folders = { "C://src", };//"C://testFlood"
	std::unordered_set<std::string> ignored_folders = { ".git", "out" };

	string raw_templ = "target text";//неотформатированный 
	string exact_templ = "";//отформатированный, например если игнорить регистр, то понижаем

	char_t joker_symbol = '*';
	char_t substring_symbol = '?';

	long long left_context = 7;
	long long right_context = 7;

	std::unordered_set<char_t> separators = { '.', ',', '!', '?', ';', ':', '(', ')',
					'[', ']', '{', '}', '"', '\'', '-', '_',
					//'«', '»', '„', '“', '”', '‘', '’', '—',
					//'…', '¿', '¡', '‹', '›', '„', '‟', '~',
					'@', '#', '$', '%', '^', '&', '*', '+',
					'=', '|', '\\', '/', '<', '>', '+', '-', '*'};

	std::atomic<bool> process_search = { true };
	std::atomic<bool> work = { true };


	static SearchConfig& get() {
		static SearchConfig instance;
		return instance;
	}

private:
	SearchConfig() = default;
	~SearchConfig() = default;
	SearchConfig(const SearchConfig&) = delete;
	SearchConfig& operator=(const SearchConfig&) = delete;
	SearchConfig(SearchConfig&&) = delete;
	SearchConfig& operator=(SearchConfig&&) = delete;

};

static bool is_separator(char_t c)  noexcept {
	if (std::isspace(static_cast<unsigned char>(c))) return true;
	return SearchConfig::get().separators.contains(c);
}