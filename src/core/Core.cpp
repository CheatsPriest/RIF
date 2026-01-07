#include <core/Core.hpp>

void Core::startFoldersWalking() {
	 inspectorPool_ptr = std::make_unique<std::jthread>([]() {
		 FoldersInspector inspector;
		 inspector.walk();
	});
}

void Core::startSearchPool() {
	searchPool_ptr = std::make_unique<ThreadSearchPool>(config.amount_of_search_threads);
}

void Core::startSeacrhing() {
	preprocessor.run();
	//std::cout << config.raw_templ << " | " << config.exact_templ << std::endl;
	stats.process_search.store(true, std::memory_order_release);
	startFoldersWalking();
}

void Core::abortSearching() {
	if (inspectorPool_ptr == nullptr or stats.process_search.load(std::memory_order_acquire))return;

	stats.process_search.store(false, std::memory_order_release);
	inspectorPool_ptr->join();
	searchPool_ptr->restartPool();
}

void Core::resizeSearchPool() {
	if (!searchPool_ptr)return;
	searchPool_ptr->resize(config.amount_of_search_threads);
}