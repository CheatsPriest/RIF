#pragma once
#include <read/UnifiedReader.hpp>
#include <configs/SearchConfig.hpp>
#include <vector>
#include <search/SearchRawResult.hpp>
#include <queue>
#include <search/synonymous/StemmerPipeline.hpp>

 
class SearchSynonymous {
private:
    SearchConfig& config;
    SynonymsSettings& settings;
    std::locale sys_locale;
    StemmerPipeline stemmer;

    std::queue<size_t> syn_in_walk;
    std::queue<size_t> ids_in_file;
    std::queue<size_t> syn_in_window;

    long long factor;
    std::vector<long long> groups;
    void cascadPop() {
        ids_in_file.pop();
        groups[syn_in_window.front()]++;
        syn_in_window.pop();
        syn_in_walk.pop();
        factor++;
    }
    void clearQueues() {
        while (!ids_in_file.empty()) {
            ids_in_file.pop();
        }
        while (!syn_in_window.empty()) {
            syn_in_window.pop();
        }
        while (!syn_in_walk.empty()) {
            syn_in_walk.pop();
        }
    }
public:
    SearchSynonymous() :config(SearchConfig::get()), settings(SynonymsSettings::get()), stemmer("russian"){

    }
    std::vector<RawResult> search(UnifiedReader& reader) {
        const auto& map = settings.synonyms_per_group;

        clearQueues();

        std::vector<RawResult> result;
        result.reserve(4);

        //то что надо обнулить но не загнать в минус
        factor = settings.target_amount;
        groups = settings.groupId_count_read_only;

        //сюда кладем позиции на которых стоят синонимы в текущем окне
        
        size_t walk_id = 0;
        
        while (!reader.empty()) {

            while (!syn_in_walk.empty() and walk_id - syn_in_walk.front() > settings.max_synonym_distance) {
                cascadPop();
            }

            string word = stemmer.stem(reader.readWord());
            walk_id++;
            
            auto it = map.find(std::move(word));

            if (it != map.end()) {
                ids_in_file.push(reader.getPos()-reader.getWordSize());
                size_t group_id = it->second;
                syn_in_window.push(group_id);
                syn_in_walk.push(walk_id);
                factor--;
                groups[group_id]--;

                while (groups[group_id] < 0) {
                    cascadPop();
                }
                if (factor == 0) {
                    result.push_back({ ids_in_file.front(), ids_in_file.back()+reader.getWordSize()});
                    //если рассинхрон 2 очередей, то значит алгоритм нетедерминированный
                    while (!ids_in_file.empty()) {
                        cascadPop();
                    }
                }

            }

            

            reader.moveToNextWord();
            
        }

        return result;
    }

};