#pragma once
#include <search/SearchExact.hpp>
#include <search/SearchSynonymous.hpp>

class SearchEngine {
private:
	SearchExact searcherExact;
	SearchSynonymous searchSynonymous;

public:
	SearchEngine() {}

	void search(const std::string& filename) {
		UnifiedReader reader(filename);

		/*auto res1 = searcherExact.search(reader);
		std::cout << "File: " << filename << " " << res1.size() << std::endl;
		for (auto pos : res1) {
			UnifiedReader contexter(filename);
			std::cout << contexter.loadContext(pos) << std::endl;
		}*/

		auto res2 = searchSynonymous.search(reader);
		std::cout << "File: " << filename << " " << res2.size() << std::endl;
		for (auto pos : res2) {
			UnifiedReader contexter(filename);
			std::cout << contexter.loadContext(pos) << std::endl;
		}
	}
};