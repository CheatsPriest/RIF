#pragma once


#include <search/ThreadSerachPool.hpp>
#include <memory>
#include <configs/SearchConfig.hpp>
#include <folders_inspector/FoldersInspector.hpp>
#include <preproc/Preprocessor.hpp>
#include <ocr/OcrManager.hpp>
class Core {
private:
	Preprocessor preprocessor;
	std::unique_ptr<ThreadSearchPool> searchPool_ptr;
	std::unique_ptr<std::jthread> inspectorPool_ptr;
	SearchConfig& config;
	SearchStats& stats;

	OcrManager& ocr_manager;

	void startFoldersWalking();
	void startSearchPool();
	
	int stop_requested_times = 0;
public:
	Core() : config(SearchConfig::get()), stats(SearchStats::get()), searchPool_ptr(nullptr), 
		inspectorPool_ptr(nullptr), ocr_manager(OcrManager::get()){
		startSearchPool();
		ocr_manager.restart();
	}
	
	void startSeacrhing();
	void abortSearching();
	void resizeSearchPool();
	~Core() {
		if(searchPool_ptr)searchPool_ptr->turnOff();
	}
};