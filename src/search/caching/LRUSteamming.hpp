#pragma once

#include <synonymous/StemmerPipeline.hpp>
#include <unordered_map>
#include <char.hpp>
#include <list>
#include <configs/SearchConfig.hpp>
#include <limits>



// Создается в SearchEngine и передается в последующие поиски
class LRUSteamming {
private:

    static constexpr size_t bad_value_flag = std::numeric_limits<size_t>::max();

    SynonymsSettings& settings;
    StemmerPipeline stemmer;

    size_t max_len;
    size_t cur_len;

    size_t cur_size;
    const size_t max_size;

    // Сразу храним принадлежность к группе
   
    
    
    std::list<string> groups;
    std::unordered_map<string_view, size_t, StringViewHash, std::equal_to<>> stem_cache;
    
    void pop_front() {
        auto& list_node = groups.front();
        

        cur_len -= list_node.size();
        cur_size--;
        stem_cache.erase(list_node);
        groups.pop_front();
    }

    void cheackValidness() {
        while (cur_len >= max_len or cur_size+1>=max_size) {
            pop_front();
        }
    }

    size_t insert(string_view word_v) {
        
        string res;// = stemmer.stem(word_v);
        auto it_c_map = settings.synonyms_per_group.find(res);
        size_t group_id = bad_value_flag;
        if (it_c_map != settings.synonyms_per_group.end()) {
            group_id = it_c_map->second;

        }
        
        cur_len += res.size();
        cur_size++;

        groups.emplace_back(std::move(res));
        stem_cache.insert( {groups.back(), group_id});
        
       
        
        //cheackValidness();

        return group_id;
        // 
    }

public:
    
    LRUSteamming(size_t max_size_= 10000, size_t max_len_ = 327680) :
        settings(SynonymsSettings::get()), max_len(max_len_), cur_len(0), 
        max_size(max_size_), cur_size(0), 
        stemmer("russian") {
        stem_cache.reserve(max_size_+1);
    }
    size_t get(string_view word) {

        auto it_s = stem_cache.find(word);
        if (it_s != stem_cache.end()) {
            return it_s->second;
        }

        return insert(word);
    }
    bool isValidId(size_t id) {
        return id != bad_value_flag;
    }
};
