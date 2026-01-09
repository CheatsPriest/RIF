#pragma once
#include <aho_corasick/SymbolsTree.hpp>
#include <array>
#include <char.hpp>

class TreeWalker {
private:
	static constexpr size_t buf_size = 128;
	SymbolsTree& tree;
	std::array<char_t, 128> buf;
	size_t len;
	node * node;
	bool verdict;
public:
	TreeWalker() : tree(SymbolsTree::get()), len(0), verdict(false)
	{
		std::fill(buf.begin(), buf.end(), '\0');
	}
	void reset() {
		len = 0;
		node = tree.getHead();
		verdict = false;
	}
	void walk(char_t c) {
		if (len < buf_size) {
			buf[len] = c;
			len++;
		}
		if (node != nullptr) {
			node = node->get(c);
			if (node!=nullptr and node->is_end_of_stem)verdict = true;
		}
	}
	bool getVerdict() const noexcept {
		return verdict;
	}
	string_view getWord() const noexcept {
		return string_view(buf.data(), len);
	}

};