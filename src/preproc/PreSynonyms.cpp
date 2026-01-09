#include <preproc/PreSynonyms.hpp>
#include <search/synonymous/StemmerPipeline.hpp>
#include <configs/SearchConfig.hpp>

#include <filesystem>
#include <string>
#include <iostream>
#include <unordered_set>
#include <sstream>
#include <fstream>
#include <read/UnifiedReader.hpp>


namespace fs = std::filesystem;

StemmerPipeline stem("russian");

class ProcessWordsFromTemplateForSynonyms {
    SearchConfig& search_config;
    SynonymsSettings& settings;
    Lowercaser lower_case;
    long long factor;
public:
    ProcessWordsFromTemplateForSynonyms() : search_config(SearchConfig::get()), settings(SynonymsSettings::get()), factor(0){

    }

    void operator()() {
        factor = 0;
        settings.words_from_template.clear();
        string cur = u"";

        for (auto c : search_config.raw_templ) {
            if (is_separator(c) and !cur.empty()) {
                //lower_case(cur);
                settings.words_from_template[stem.stem(std::move(cur))]++;
                cur.clear();
                factor++;
            }
            else if (!is_separator(c)) {
                cur += c;
            }

        }
        if (!cur.empty()) {
            lower_case(cur);
            settings.words_from_template[stem.stem(std::move(cur))]++;
            factor++;
            cur.clear();
        }
        settings.target_amount = factor;
    }

};

class SynonymsReader {
    SynonymsSettings& settings;
    StemmerPipeline stem;

    struct group_info {
        long long count;
        size_t future_id;
    };

    std::unordered_map<size_t, group_info> temp_groups;
    size_t free_group_id;

public:
    SynonymsReader() : settings(SynonymsSettings::get()), stem("russian") {}

    void operator()(const std::string& folder_path = "C:\\testFlood\\synonyms") {
        free_group_id = 0;
        if (read_from_folder(folder_path)) {
            settings.words_from_template.clear();
            settings.max_group_id = free_group_id;
        }
    }

private:
    bool read_from_folder(const std::string& folder_path) {
        namespace fs = std::filesystem;
        if (!fs::exists(folder_path)) return false;

        for (const auto& lang_dir : fs::directory_iterator(folder_path)) {
            if (!fs::is_directory(lang_dir)) continue;

            stem.changeLanguage(lang_dir.path().filename().string());

            for (const auto& file : fs::directory_iterator(lang_dir.path())) {
                if (file.path().extension() != ".txt") continue;

                // Первый проход: считаем частоты и создаем ID групп
                {
                    UnifiedReader reader(file.path().string());
                    form_groups(reader);
                }

                // Второй проход: заполняем карту синонимов
                {
                    UnifiedReader reader(file.path().string());
                    processGroups(reader);
                }
            }
        }
        return true;
    }

    // Вспомогательная функция для чтения ID (так как UnifiedReader читает слова)
    size_t parseId(const string& word) {
        if (word.empty()) return 0;
        try {
            // Конвертируем UTF-16 обратно в UTF-8 для std::stoull
            std::string s;
            icu::UnicodeString(reinterpret_cast<const UChar*>(word.data()), word.size()).toUTF8String(s);
            return std::stoull(s);
        }
        catch (...) { return 0; }
    }

    void form_groups(UnifiedReader& reader) {
        while (!reader.empty()) {
            string id_str = string(reader.readWord());
            if (id_str.empty()) break;
            
            size_t group_id = parseId(id_str);
            string word = string(reader.readWord());
            if (word.empty()) continue;

            word = stem.stem(std::move(word));

            auto it = settings.words_from_template.find(std::move(word));
            if (it != settings.words_from_template.end()) {
                auto& g_info = temp_groups[group_id];
                if (g_info.count == 0 && g_info.future_id == 0) {
                    // Новая группа
                    g_info = { it->second, free_group_id++ };
                }
                else {
                    g_info.count += it->second;
                }
            }
        }
    }

    void processGroups(UnifiedReader& reader) {
        while (!reader.empty()) {
            string id_str = string(reader.readWord());
            if (id_str.empty()) break;

            size_t group_id = parseId(id_str);
            string word = string( reader.readWord());
            if (word.empty()) continue;

            auto it = temp_groups.find(group_id);
            if (it != temp_groups.end()) {
                settings.synonyms_per_group.insert({
                    stem.stem(std::move(word)),
                    it->second.future_id
                    });
            }
        }

        // Синхронизация вектора частот
        settings.groupId_count_read_only.resize(free_group_id);
        for (auto const& [old_id, info] : temp_groups) {
            settings.groupId_count_read_only[info.future_id] = info.count;
        }
        temp_groups.clear();
    }
};

void PreSynonyms::run() {
    SynonymsSettings::get().flushDown();
    if (!SynonymsSettings::get().use_synonyms)return;
    ProcessWordsFromTemplateForSynonyms pr;
    pr();
    SynonymsReader sr;
    sr();

}