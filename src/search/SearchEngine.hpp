#pragma once
#include <search/SearchExact.hpp>
#include <search/SearchSynonymous.hpp>
#include <global/GlobalQueues.hpp>

class SearchEngine {
private:
	SearchExact searcherExact;
	SearchSynonymous searchSynonymous;
	SearchConfig& config;
	SearchStats& stats;
	result_queue_t& result_queue;
public:
	SearchEngine() : stats(SearchStats::get()), config(SearchConfig::get()), result_queue(ResultQueue::get()){}

    void search(const std::filesystem::path& filename);
};