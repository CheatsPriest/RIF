#pragma once
#include <configs/SearchConfig.hpp>
#include <vector>
#include <unordered_map>

#include <char.hpp>

#define AhoMap

#ifdef AhoMap
struct node {
	bool is_end_of_stem = false;
	std::unordered_map<char_t, std::unique_ptr<node>> childs;
	
	node* get(char_t c) {
		auto it = childs.find(c);
		if (it != childs.end()) {
			return it->second.get();
		}
		else {
			return nullptr;
		}
	}
};
#else
struct node {
	bool is_end_of_stem = false;
	std::vector<std::pair<char_t, std::unique_ptr<node>>> childs;

	node* get(char_t c) {
		for (auto& child : childs) {
			if (child.first == c) {
				return child.second.get();
			}
		}
		return nullptr;
	}
};
#endif

class SymbolsTree {
private:
	
	SynonymsSettings& settings;
	
	std::unique_ptr<node> head;

	void internalBuild(const string& word) {
		if (!head) head = std::make_unique<node>();

		node* cur_node = head.get(); // Начинаем с корня
#ifdef AhoMap
		for (char_t c : word) {
			auto it = cur_node->childs.find(c);

			if (it == cur_node->childs.end()) {
				// Если узла нет, создаем его
				// Используем [] или insert/emplace, но без копирования
				cur_node->childs[c] = std::make_unique<node>();
				cur_node = cur_node->childs[c].get();
			}
			else {
				// Если узел есть, просто переходим
				cur_node = it->second.get();
			}
		}
#else
		for (char_t c : word) {
			node* it = cur_node->get(c);

			if (it == nullptr) {
				// Если узла нет, создаем его
				// Используем [] или insert/emplace, но без копирования
				cur_node->childs.push_back({ c, std::make_unique<node>() });
				cur_node = cur_node->get(c);
			}
			else {
				// Если узел есть, просто переходим
				cur_node = it->get(c);
			}
		}
#endif
		cur_node->is_end_of_stem = true;
	}




public:
	
	static SymbolsTree& get() {
		static SymbolsTree instance;

		return instance;
	}
	
	void buildTree() {
		head.reset();
		head = std::make_unique<node>();
		for (auto& [word, group] : settings.synonyms_per_group) {
			internalBuild(word);
		}

	}

	node* getHead() {
		return head.get();
	}

private:
	SymbolsTree() : settings(SynonymsSettings::get()) {

	}
	~SymbolsTree() = default;
	SymbolsTree(const SymbolsTree&) = delete;
	SymbolsTree& operator=(const SymbolsTree&) = delete;
	SymbolsTree(SymbolsTree&&) = delete;
	SymbolsTree& operator=(SymbolsTree&&) = delete;
};

