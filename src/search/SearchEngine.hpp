#pragma once
#include <search/SearchExact.hpp>
#include <search/SearchSynonymous.hpp>

class SearchEngine {
private:
	SearchExact searcherExact;
	SearchSynonymous searchSynonymous;
    
public:
	SearchEngine() {}

    void search(const std::string& filename);
};