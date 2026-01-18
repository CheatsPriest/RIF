#include <ocr/OcrManager.hpp>
#include <ocr/OcrReader.hpp>

#include <fstream>
#include <filesystem>

#include <ICU/Decoders.hpp>

const char* cached_ocr_path = "C:/RIF/";

void save_to_cache(std::stop_token stoken, const std::string& cache_path, const std::string& file_path) {

	std::filesystem::create_directories(std::filesystem::path(cache_path).parent_path());

	std::ofstream out(cache_path, std::ios::out | std::ios::binary);

	if (out.is_open() and !stoken.stop_requested()) {
		try {
			OcrReader reader(file_path);
			std::string_view chunk;
			while (reader.readNextChunk(chunk)) {
				if (stoken.stop_requested()) {
					out.close();
					std::filesystem::remove(cache_path);
					std::cout << "Leaved" << std::endl;
					return;
				}
				
				out.write(chunk.data(), chunk.size());
				out.flush();
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

size_t OcrManager::get_cache_key(const std::filesystem::path& fs_path)
 {
		auto size = std::filesystem::file_size(fs_path);
		auto mtime = std::filesystem::last_write_time(fs_path).time_since_epoch().count();
		auto name = fs_path.filename().string();
		std::string raw_key = std::format("{}|{}|{}", name, size, mtime);
		return std::hash<std::string>{}(raw_key);
}

void OcrManager::work(std::stop_token stoken) {
	size_t cur_key;
	while (!stoken.stop_requested()) {

		if (!to_process.pop(cur_key))break;


		auto& cur_entry = map[cur_key];
		
		auto u8str = cur_entry.source_file_path.u8string();
		auto str = normalizeU8ToStd(u8str);

		auto cache_file_path = std::format("{}[{}[{}.chd", cached_ocr_path, cur_entry.source_file_path.filename().string(), cur_key);

		
		save_to_cache(stoken, cache_file_path, str);
		if (stoken.stop_requested())break;

		cur_entry.cache_file_path = cache_file_path;
		cur_entry.state = OcrCacheState::READY;

		stats.files_to_process.fetch_add(1, std::memory_order::seq_cst);
		stats.in_ocr_process.fetch_sub(1, std::memory_order::seq_cst);

		files_queue.push(std::move(cache_file_path));

	}

}

void OcrManager::loadCache() {
	namespace fs = std::filesystem;

	if (!fs::exists(cached_ocr_path)) return;

	for (const auto& entry : fs::directory_iterator(cached_ocr_path)) {
		if (!entry.is_regular_file() || entry.path().extension() != ".chd")
			continue;

		std::string filename = entry.path().filename().string();

		// Ищем позиции разделителей '[' с конца
		size_t last_pipe = filename.find_last_of('[');
		size_t ext_dot = filename.find_last_of('.');

		if (last_pipe != std::string::npos && ext_dot != std::string::npos) {
			try {
				// Извлекаем ключ (тот самый size_t cur_key)
				std::string key_str = filename.substr(last_pipe + 1, ext_dot - last_pipe - 1);
				size_t key = std::stoull(key_str);

				// Формируем запись
				CacheEntry cached_entry;
				cached_entry.cache_file_path = entry.path().string();
				cached_entry.state = OcrCacheState::READY;
				// source_file_path восстановится полностью только при первой встрече 
				// реального файла в поисковике, пока можем оставить пустым или записать имя

				map.insert(key, std::move(cached_entry));
			}
			catch (...) {
				// Если файл поврежден или имя не по формату — игнорируем
				continue;
			}
		}
	}
}

void OcrManager::push(const std::filesystem::path& path) {

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