#pragma once


#include <search/ThreadSerachPool.hpp>
#include <memory>
#include <configs/SearchConfig.hpp>
#include <folders_inspector/FoldersInspector.hpp>
#include <preproc/Preprocessor.hpp>
class Core {
private:
	Preprocessor preprocessor;
	std::unique_ptr<ThreadSearchPool> searchPool_ptr;
	std::unique_ptr<std::jthread> inspectorPool_ptr;
	SearchConfig& config;

	void startFoldersWalking();
	void startSearchPool();
public:
	Core() : config(SearchConfig::get()), searchPool_ptr(nullptr), inspectorPool_ptr(nullptr){
		startSearchPool();
	}
	
	void startSeacrhing();
	void abortSearching();
	void resizeSearchPool();
	~Core() {
		if(searchPool_ptr)searchPool_ptr->turnOff();
	}
};