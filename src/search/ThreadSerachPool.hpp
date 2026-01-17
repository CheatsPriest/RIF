#pragma once
#include <read/UnifiedReader.hpp>
#include <configs/SearchConfig.hpp>
#include <global/GlobalQueues.hpp>
#include <search/SearchEngine.hpp>
#include <vector>
#include <thread>



class ThreadSearchPool {
private:
    
    file_queue_t& files_q;
    result_queue_t& result_q;
    SearchConfig& config;
    size_t size;
    std::vector<std::jthread> pool;

    void work(std::stop_token stoken);

public:
    ThreadSearchPool(size_t size) : size(size), files_q(FilesQueues::get()), result_q(ResultQueue::get()), 
        config(SearchConfig::get()){
        pool.reserve(size);

        for (size_t i = 0; i < size; i++) {
            pool.emplace_back([this](std::stop_token token) {
                this->work(token);  
                });
        }
        
    }

    void restartPool() {
        //resize(size);
        SearchStats::get().files_processed+=files_q.clear();
        //result_q.clear();
    }

    void resize(size_t new_size);

    void stopPool();
    void turnOff();
};