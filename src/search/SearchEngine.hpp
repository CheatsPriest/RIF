#pragma once
#include <search/SearchExact.hpp>
class SearchEngine {
private:
	SearchExact searcherExact;

public:
	SearchEngine() {}

	void search(const std::string& filename) {
		UnifiedReader reader(filename);

		auto res = searcherExact.search(reader);
		std::cout << "File: " << filename << " " << res.size() << std::endl;
		for (auto pos : res) {
			UnifiedReader contexter(filename);
			std::cout << contexter.loadContext(pos) << std::endl;
		}
	}
};