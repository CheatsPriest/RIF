#pragma once
#include <read/UnifiedReader.hpp>
#include <configs/SearchConfig.hpp>
#include <vector>
#include <global/GlobalQueues.hpp>
#include <queue>

#include <synonymous/DeductorStemmer.hpp>

#include <aho_corasick/TreeWalker.hpp>
// Продвинутый поиск
class SearchSynonymous {
private:
    SearchConfig& config;
    SynonymsSettings& settings;
    DeductorStemmer stemmer;

    TreeWalker treeWalker;

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
    SearchSynonymous() :config(SearchConfig::get()), settings(SynonymsSettings::get()){

    }
    std::vector<ConcretePlace> search(UnifiedReader& reader) {
        const auto& map = settings.synonyms_per_group;
        reader.setIsLowercase(true);
        clearQueues();

        std::vector<ConcretePlace> result;
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


            
            walk_id++;
            
            treeWalker.reset();

            reader.skipSeparators();
            
            while (!reader.empty()) {
                char_t c = reader.readSymbol();
                if (is_separator(c))break;
                treeWalker.walk(c);
                reader.moveToSymbol(1);
            }

            if (!treeWalker.getVerdict())continue;
            auto word = stemmer.stem_lowercased(treeWalker.getWord());


            auto it = map.find(word);

            if (it != map.end()) {
                ids_in_file.push(reader.getPos()-word.size());
                size_t group_id = it->second;
                syn_in_window.push(group_id);
                syn_in_walk.push(walk_id);
                factor--;
                groups[group_id]--;

                while (groups[group_id] < 0) {
                    cascadPop();
                }
                if (factor == 0) {
                    result.push_back({ ids_in_file.front(), ids_in_file.back()+ word.size() });
                    //если рассинхрон 2 очередей, то значит алгоритм нетедерминированный
                    while (!ids_in_file.empty()) {
                        cascadPop();
                    }
                }

            }

            
        }

        return result;
    }

};