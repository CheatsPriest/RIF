#include <search/SearchEngine.hpp>
#include <ICU/Decoders.hpp>

void mergeTwoVectors(const std::vector<std::vector<ConcretePlace>>& buf_vector, std::vector<ConcretePlace>& final_vector) {
    if (buf_vector.empty()) return;
    if (buf_vector.size() == 1) {
        final_vector = buf_vector[0];
        return;
    }

    const auto& v1 = buf_vector[0];
    const auto& v2 = buf_vector[1];

    final_vector.clear();
    final_vector.reserve(v1.size() + v2.size());

    size_t i = 0, j = 0;

    while (i < v1.size() && j < v2.size()) {
        ConcretePlace current;

        if (v1[i] < v2[j]) {
            current = v1[i++];
        }
        else if (v2[j] < v1[i]) {
            current = v2[j++];
        }
        else {
            current = v1[i++];
            j++;
        }

        final_vector.push_back(current);
    }

    while (i < v1.size()) final_vector.push_back(v1[i++]);
    while (j < v2.size()) final_vector.push_back(v2[j++]);
}

void SearchEngine::search(const std::filesystem::path& filename) {

	std::vector<std::vector<ConcretePlace>> buf_vector;
	std::vector<ConcretePlace> final_vector;

    UnifiedReader reader(filename, !config.respect_registers);
	{
		auto res1 = searcherExact.search(reader);
		buf_vector.push_back(std::move(res1));
        reader.rewind();
	}
	if (SynonymsSettings::get().use_synonyms and SynonymsSettings::get().synonyms_per_group.size() != 0) {
        reader.setIsLowercase(true);
		auto res2 = searchSynonymous.search(reader);
		buf_vector.push_back(std::move(res2));
	}

    //mergeTwoVectors(buf_vector, final_vector);
    for (auto& el : buf_vector) {
        for (auto& res : el) {
            final_vector.push_back(res);
        }
    }

    if (!final_vector.empty()) {
        SearchResult result;
        
        auto u8s = filename.u8string();
        result.file = normalizeU8ToStd(u8s);

        result.file_string_for_reader = filename.string();

        result.places = std::move(final_vector);
        result_queue.push(std::move(result));
    }

	/*if (final_vector.size() != 0) {
		std::cout << "File: " << filename << " === " << final_vector.size() << std::endl;
		for (auto& pos : final_vector) {
			UnifiedReader contexter(filename);
			std::cout << contexter.loadContext(pos.left, pos.right) << std::endl;
		}
	}*/
    stats.files_processed.fetch_add(1, std::memory_order::release);
    stats.checkStatus();
}