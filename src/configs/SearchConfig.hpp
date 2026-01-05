// SearchConfig.hpp
#pragma once
#include <atomic>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

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

	string raw_templ = "огромные деньги";//неотформатированный 
	string exact_templ = "";//отформатированный, например если игнорить регистр, то понижаем

	char_t joker_symbol = '*';
	char_t substring_symbol = '?';

	long long left_context = 25;
	long long right_context = 25;

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

struct SynonymsSettings {
	bool use_synonyms = true;
	size_t max_synonym_distance = 5;

	std::unordered_map<string, long long> words_from_template;
	/*
	Синонимы для ключевых терминов
	 Очистить после обработки по группам
	*/
	std::vector<std::vector<string>> raw_synonyms;
	/*
	* Обратная модель учета частот
	* локлаьную веерсию target_amount надо свестить к 0, только тогда мы получаем
	* ничего менять нельзя
	* возня с id ради cache hits и гарантом O(1)
	*/
	long long target_amount;
	size_t max_group_id;
	std::vector<long long> groupId_count_read_only;
	std::unordered_map<string, size_t> synonyms_per_group;

	// Стоп-слова для пропуска
	std::unordered_set<string> skip_words = {
		"в", "на", "за", "под", "над", "и", "или", "но",
		"с", "по", "о", "от", "до", "из", "у", "без"
	};
	bool isSkipabble(const string& word) {
		return skip_words.contains(word);
	}
	static SynonymsSettings& get() {
		static SynonymsSettings instance;
		return instance;
	}
	void flushDown() {
		synonyms_per_group.clear();
		groupId_count_read_only.clear();
		target_amount = 0;
		raw_synonyms.clear();
		words_from_template.clear();
	}
private:
	SynonymsSettings() = default;
	~SynonymsSettings() = default;
	SynonymsSettings(const SynonymsSettings&) = delete;
	SynonymsSettings& operator=(const SynonymsSettings&) = delete;
	SynonymsSettings(SynonymsSettings&&) = delete;
	SynonymsSettings& operator=(SynonymsSettings&&) = delete;

};

static bool is_separator(char_t c)  noexcept {
	if (std::isspace(static_cast<unsigned char>(c))) return true;
	return SearchConfig::get().separators.contains(c);
}