// SearchConfig.hpp
#pragma once
#include <atomic>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include <char.hpp>
#include <unicode/uchar.h>
#include <filesystem>
namespace fs = std::filesystem;

constexpr static int start_amount_of_search_threads = 1;

class SearchConfig {
public:
	
	int amount_of_search_threads = start_amount_of_search_threads;

	int depth = 0;
	bool respect_registers = false;
	bool full_words = false;
	bool use_ocr_for_pdf = true;


	std::unordered_set<std::string> allowed_extensions = { ".txt", ".md", ".cpp", ".c", ".hpp", ".h", ".docx", ".pdf"};

	std::vector<fs::path> initial_folders = { R"(C:\src)", };//"C://testFlood"
	//std::vector<std::string> initial_folders = { R"(C:\Users\kuzne\Desktop\charshtest)", };//"C://testFlood"
	std::unordered_set<std::string> ignored_folders = { ".git", "out" };

	string raw_templ = u"text";//неотформатированный 
	string exact_templ = u"text";//отформатированный, например если игнорить регистр, то понижаем

	char_t joker_symbol = '*';
	char_t substring_symbol = '?';

	int left_context = 25;
	int right_context = 25;

	std::unordered_set<char_t> separators = { '.', ',', '!', '?', ';', ':', '(', ')',
					'[', ']', '{', '}', '"', '\'', '-', '_',
					u'«', u'»', u'„', u'“', u'”', u'‘', u'’', u'—',
					u'…', u'¿', u'¡', u'‹', u'›', u'„', u'‟', '~',
					'@', '#', '$', '%', '^', '&', '*', '+',
					'=', '|', '\\', '/', '<', '>', '+', '-', '*'};

	


	
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

struct StringViewHash {
	using is_transparent = void; // Важно для C++20
	size_t operator()(string_view sv) const { return std::hash<string_view>{}(sv); }
	size_t operator()(const string& s) const { return std::hash<string>{}(s); }
};

struct SynonymsSettings {
	bool use_synonyms = true;
	size_t max_synonym_distance = 5;

	std::unordered_map<string, long long, StringViewHash, std::equal_to<>> words_from_template;
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
	std::unordered_map<string, size_t, StringViewHash, std::equal_to<>> synonyms_per_group;

	// Стоп-слова для пропуска
	std::unordered_set<string> skip_words = {
		u"в", u"на", u"за", u"под", u"над", u"и", u"или", u"но",
		u"с", u"по", u"о", u"от", u"до", u"из", u"у", u"без"
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

static bool is_separator(char_t c) noexcept {
	// Используем ICU для проверки пробельных символов Unicode
	if (u_isUWhiteSpace(static_cast<UChar32>(c))) return true;
	return SearchConfig::get().separators.contains(c);
}

struct SearchStats {
public:

	alignas(64) std::atomic<bool> process_search = { false };
	alignas(64) std::atomic<bool> work = { true };

	alignas(64) std::atomic<bool> is_inspecting_folders{ false };
	alignas(64) std::atomic<long long> files_to_process{ 0 };
	alignas(64) std::atomic<long long> files_processed{ 0 };

	alignas(64) std::atomic<size_t> kbytes_read{ 0 };

	alignas(64) std::atomic<size_t> in_ocr_process{ 0 };

	static SearchStats& get() {
		static SearchStats instance;
		return instance;
	}
	void reset() {
		is_inspecting_folders = false;
		files_to_process = 0;
		files_processed = 0;
		kbytes_read = 0;
	}
	void checkStatus() {

		if (is_inspecting_folders.load(std::memory_order_acquire)) {
			return;
		}

		// 1. Читаем атомики поиска
		long long to_do = files_to_process.load(std::memory_order_acquire);
		long long done = files_processed.load(std::memory_order_acquire);

		// 2. Читаем OCR атомики
		size_t in_ocr = in_ocr_process.load(std::memory_order_acquire);

		// 3. Вердикт: Поиск закончен ТОЛЬКО если инспектор ушел И счетчики сошлись и OCR не работает
		if (to_do == done and in_ocr == 0) {
			process_search.store(false, std::memory_order_release);
		}
	}
private:
	SearchStats() = default;
	~SearchStats() = default;
	SearchStats(const SearchStats&) = delete;
	SearchStats& operator=(const SearchStats&) = delete;
	SearchStats(SearchStats&&) = delete;
	SearchStats& operator=(SearchStats&&) = delete;
};