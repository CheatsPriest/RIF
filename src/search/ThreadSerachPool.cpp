#include <search/ThreadSerachPool.hpp>

void ThreadSearchPool::work(std::stop_token stoken) {
    SearchEngine eng;
    while (!stoken.stop_requested()) {
        std::string file_name;

        if (!files_q.pop(file_name))break;

        std::cout << file_name << std::endl;
        eng.search(file_name);

    }

}

void ThreadSearchPool::resize(size_t new_size) {
    size = new_size;

    stopPool();
    pool.clear();
    pool.reserve(size);
    for (size_t i = 0; i < size; i++) {
        pool.emplace_back([this](std::stop_token token) {
            this->work(token); 
            });
    }
}

void ThreadSearchPool::stopPool() {
    files_q.turnOff();
    files_q.clear();
    for (auto& el : pool) {
        el.request_stop();
    }
    for (auto& el : pool) {
        if (el.joinable())el.join();
    }
    files_q.turnOn();

    result_q.clear();
}

void ThreadSearchPool::turnOff() {
    stopPool();
    files_q.turnOff();
    result_q.turnOff();
}