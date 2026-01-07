#pragma once
#include <search/SearchExact.hpp>
#include <search/SearchSynonymous.hpp>

class SearchEngine {
private:
	SearchExact searcherExact;
	SearchSynonymous searchSynonymous;
	SearchStats& stats;
public:
	SearchEngine() : stats(SearchStats::get()){}

    void search(const std::string& filename);
};