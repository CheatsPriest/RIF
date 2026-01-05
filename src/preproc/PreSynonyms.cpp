#include <preproc/PreSynonyms.hpp>
#include <search/synonymous/StemmerPipeline.hpp>
#include <configs/SearchConfig.hpp>

#include <filesystem>
#include <string>
#include <iostream>
#include <unordered_set>
#include <sstream>
#include <fstream>


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
        string cur = "";

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

struct group_info {
    long long count;
    size_t future_id;
};

class SynonymsReader {
    SynonymsSettings& settings;
    std::unordered_map<size_t, group_info> temp_groups;
    size_t free_group_id;
public:
    SynonymsReader() : settings(SynonymsSettings::get()) {}

    void operator()(const std::string& folder_path = "C:\\testFlood\\synonyms") {
        read_from_folder(folder_path);
        settings.words_from_template.clear();
        settings.max_group_id = free_group_id;
    }

    

private:
    bool read_from_folder(const std::string& folder_path) {
        namespace fs = std::filesystem;
        free_group_id = 0;

        if (!fs::exists(folder_path)) {
            std::cerr << "Папка " << folder_path << " не найдена!" << std::endl;
            return false;
        }

        // Проходим по всем языковым папкам
        for (const auto& lang_dir : fs::directory_iterator(folder_path)) {
            if (!fs::is_directory(lang_dir)) continue;

            std::string language = lang_dir.path().filename().string();

            stem.changeLanguage(language);

            for (const auto& file : fs::directory_iterator(lang_dir.path())) {
                if (file.path().extension() != ".txt") continue;

                std::ifstream fin(file.path());
                if (!fin.is_open()) {
                    std::cerr << "Не удалось открыть: " << file.path() << std::endl;
                    continue;
                }

                form_groups(fin);
                fin.close();

                std::ifstream fina(file.path());
                if (!fina.is_open()) {
                    std::cerr << "Не удалось открыть: " << file.path() << std::endl;
                    continue;
                }
                processGroups(fina);
                fina.close();


            }
        }

        return true;
    }

    void form_groups(std::istream& input) {
        std::string line;
        temp_groups.clear();

        while (std::getline(input, line)) {
            std::istringstream iss(line);
            size_t group_id;
            std::string word;

            if (iss >> group_id) {
                std::getline(iss >> std::ws, word);
                word = stem.stem(formatToLocal(std::move(word)));
                auto it = settings.words_from_template.find(std::move(word));
                if (it != settings.words_from_template.end()) {
                    //Сколько раз слова из этой группы должны встретиться
                    auto it_g = temp_groups.find(group_id);
                    if (it_g != temp_groups.end()) {
                        temp_groups[group_id].count+= it->second;
                    }
                    else {
                        temp_groups.insert({ group_id, {it->second , free_group_id++} });
                    }
                }
            }
        }

    }
    void processGroups(std::istream& input){
        std::string line;
        // ВТОРОЙ ПРОХОД: создаём наборы синонимов
        while (std::getline(input, line)) {
            std::istringstream iss(line);
            size_t group_id;
            std::string word;

            if (iss >> group_id) {
                std::getline(iss >> std::ws, word);
                word = formatToLocal(std::move(word));

                auto it = temp_groups.find(group_id);
                if (it != temp_groups.end()) {
                    settings.synonyms_per_group.insert({ stem.stem(std::move(word)), it->second.future_id });
                }

            }
        }
        settings.groupId_count_read_only.resize(free_group_id);
        for (auto& [old_id, info] : temp_groups) {
            settings.groupId_count_read_only[info.future_id] = info.count;
        }

        temp_groups.clear();
    }
};

class SynonymsLoader {
private:
	

public:

};

void PreSynonyms::run() {
    SynonymsSettings::get().flushDown();
    if (!SynonymsSettings::get().use_synonyms)return;
    ProcessWordsFromTemplateForSynonyms pr;
    pr();
    SynonymsReader sr;
    sr();

}