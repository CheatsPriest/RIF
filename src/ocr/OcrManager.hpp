#pragma once
#include <ocr/MutexMap.hpp>
#include <global/GlobalQueues.hpp>
#include <configs/SearchConfig.hpp>

#include <string>
#include <filesystem>
#include <format>

#include <vector>
#include <thread>

enum class OcrCacheState : uint16_t
{
	ABSENT,
	IN_PROGRESS,
	READY
};

struct CacheEntry {
	std::filesystem::path source_file_path;
	std::string cache_file_path;
	OcrCacheState state = OcrCacheState::IN_PROGRESS;
};

class OcrManager {
private:
	MutexMap<size_t, CacheEntry>  map;
	SearchStats& stats;

	size_t get_cache_key(const std::filesystem::path& fs_path);

	ThreadSafeQueue<size_t> to_process;
	file_queue_t& files_queue;


	size_t size;
	std::vector<std::jthread> pool;

	void work(std::stop_token stoken);
	std::atomic<bool> does_it_work;

	void loadCache();

public:
	
	static OcrManager& get() {
		static OcrManager instance;
		return instance;
	}

	void push(const std::filesystem::path& path);

	void launchIfNeed() {
		if (!does_it_work.load(std::memory_order_acquire)) {
			restart();
			does_it_work.store(true, std::memory_order_release);
		}
	}
	
	void stop() {
		if (does_it_work.load(std::memory_order_acquire)) {
			to_process.turnOff();
			for (auto& el : pool) {
				el.request_stop();
			}
			for (auto& el : pool) {
				el.join();
			}
			pool.clear();
			to_process.turnOn();
			does_it_work.store(false, std::memory_order_release);
		}
		to_process.clear();
	}

	void restart() {
		stop();
		pool.reserve(size);
		for (size_t i = 0; i < size; i++) {
			pool.emplace_back([this](std::stop_token token) {
				this->work(token);
				});
		}
		stats.in_ocr_process = 0;
		stats.checkStatus();
	}
	void resizeAndRestart(size_t new_size) {
		size = new_size;
		restart();
	}

	
private:
	OcrManager() : map(128), stats(SearchStats::get()), to_process(64), files_queue(FilesQueues::get()), size(2), does_it_work(false) {
		loadCache();
	}
	OcrManager(const OcrManager&) = delete;
	OcrManager(OcrManager&&) = delete;

	OcrManager& operator=(const OcrManager&) = delete;
	OcrManager& operator=(OcrManager&&) = delete;

	~OcrManager() {
		to_process.turnOff();
	}
};