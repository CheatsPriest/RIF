#include <search/ThreadSerachPool.hpp>

void ThreadSearchPool::work(std::stop_token stoken) {
    SearchEngine eng;

    while (true) {
        if (stoken.stop_requested()) {
            break;
        }

        std::filesystem::path file_name;

        // pop() вернет false если очередь выключена
        if (!files_q.pop(file_name)) {
            break;
        }

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
    result_q.turnOff();

    for (auto& el : pool) {
        el.request_stop();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    files_q.clear();
    result_q.clear();

    for (auto& el : pool) {
        if (el.joinable()) {
            el.join();
        }
    }

    pool.clear();
    files_q.turnOn();
    result_q.turnOn();
}

void ThreadSearchPool::turnOff() {
    stopPool();
    files_q.turnOff();
    result_q.turnOff();
}