#include <ocr/OcrManager.hpp>
#include <ocr/OcrReader.hpp>

#include <fstream>
#include <filesystem>

void save_to_cache(std::stop_token stoken, const std::string& cache_path, const std::string& file_path) {

	std::filesystem::create_directories(std::filesystem::path(cache_path).parent_path());

	std::ofstream out(cache_path, std::ios::out | std::ios::binary);

	if (out.is_open() and !stoken.stop_requested()) {
		try {
			OcrReader reader(file_path);
			string chunk;
			while (reader.readNextChunk(chunk)) {
				if (stoken.stop_requested()) {
					out.close();
					std::filesystem::remove(cache_path);
					return;
				}
				out << chunk;
			}
		}
		catch (...) {

		}
		out.close();
	}
	else {
		std::cerr << "Failed to create file: " << cache_path << std::endl;
	}
}

void OcrManager::work(std::stop_token stoken) {
	size_t cur_key;
	while (!stoken.stop_requested()) {

		if (!to_process.pop(cur_key))break;


		auto& cur_entry = map[cur_key];
		auto fs_path = std::filesystem::path(cur_entry.source_file_path);
		
		auto cache_file_path = std::format("{}{}{}.chd", "C:/RIF/", fs_path.filename().string(), cur_key);

		save_to_cache(stoken, cache_file_path, cur_entry.source_file_path);
		if (stoken.stop_requested())break;

		cur_entry.cache_file_path = cache_file_path;
		cur_entry.state = OcrCacheState::READY;

		stats.files_to_process.fetch_add(1, std::memory_order::seq_cst);
		stats.in_ocr_process.fetch_sub(1, std::memory_order::seq_cst);

		files_queue.push(std::move(cache_file_path));

	}

}

void OcrManager::push(const std::string& path) {

	size_t cache_key = get_cache_key(path);
	if (map.contains(cache_key)) {
		auto& cur = map[cache_key];
		if (cur.state != OcrCacheState::READY)return;
		files_queue.push(cur.cache_file_path);
		return;
	}
	
	CacheEntry cur;
	cur.source_file_path = path;

	stats.in_ocr_process.fetch_add(1, std::memory_order::seq_cst);
	map.insert(cache_key, cur);
	to_process.push(cache_key);
	

}