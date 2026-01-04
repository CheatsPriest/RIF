#pragma once
#include "ThreadSafeQueue.hpp"
#include <string>

struct SearchResult {
    std::string file;
    size_t position;
    string context;
    SearchResult(size_t position) : position(position){}
};

using file_queue_t = ThreadSafeQueue<std::string>;
using result_queue_t = ThreadSafeQueue<SearchResult>;

class FilesQueues {
private:

    file_queue_t queue;

public:
    static file_queue_t& get() noexcept {
        static FilesQueues instance;
        return instance.queue;
    }
private:

    FilesQueues() = default;

    ~FilesQueues() = default;
    FilesQueues(const FilesQueues&) = delete;
    FilesQueues& operator=(const FilesQueues&) = delete;
    FilesQueues(FilesQueues&&) = delete;
    FilesQueues& operator=(FilesQueues&&) = delete;
};



class ResultQueue {
private:

    result_queue_t queue;

public:
    static result_queue_t& get() noexcept {
        static ResultQueue instance;
        return instance.queue;
    }
private:

    ResultQueue() = default;

    ~ResultQueue() = default;
    ResultQueue(const ResultQueue&) = delete;
    ResultQueue& operator=(const ResultQueue&) = delete;
    ResultQueue(ResultQueue&&) = delete;
    ResultQueue& operator=(ResultQueue&&) = delete;
};

