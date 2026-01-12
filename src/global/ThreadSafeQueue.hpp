#pragma once
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <queue>

static constexpr size_t qSize = 128;

template<typename T>
class ThreadSafeQueue {
private:
    size_t capacity;
    size_t size;
    std::queue<T> q;

    std::atomic<bool> work;

    std::mutex q_mtx;
    std::condition_variable in_cv, out_cv;

public:

    ThreadSafeQueue(size_t capacity = qSize) : capacity(capacity), size(0), work(true) {}

    bool empty() {
        std::lock_guard< std::mutex> lock(q_mtx);
        return q.empty();
    }
    void push(const T& val) {
        std::unique_lock<std::mutex> lock(q_mtx);
        in_cv.wait(lock, [this]() { return size < capacity or !work.load(std::memory_order::acquire); });

        if (!work.load(std::memory_order::acquire))return;

        q.push(val);
        size++;

        lock.unlock();
        out_cv.notify_one();
    }
    void push(T&& val) {
        std::unique_lock<std::mutex> lock(q_mtx);
        in_cv.wait(lock, [this]() { return size < capacity or !work.load(std::memory_order::acquire); });

        if (!work.load(std::memory_order::acquire))return;

        q.push(std::move(val));
        size++;

        lock.unlock();
        out_cv.notify_one();
    }
    bool pop(T& in) {
        std::unique_lock<std::mutex> lock(q_mtx);
        out_cv.wait(lock, [this]() { return !q.empty() or !work.load(std::memory_order::acquire); });

        if (!work.load(std::memory_order::acquire))return false;

        in = std::move(q.front());
        q.pop();

        size--;

        lock.unlock();
        in_cv.notify_one();
        return true;
    }
    T unwait_pop() {
        std::unique_lock<std::mutex> lock(q_mtx);
       
        T res = std::move(q.front());
        q.pop();
        size--;
        lock.unlock();

        in_cv.notify_one();

        return res;

    }
    bool isValid()const {
        return work;
    }
    void turnOff() {
        work.store(false, std::memory_order::release);
        in_cv.notify_all();
        out_cv.notify_all();

    }
    void turnOn() {
        work.store(true);
    }
    void clear() {
        std::unique_lock<std::mutex> lock(q_mtx);
        while (!q.empty()) {
            q.pop();
        }
    }
};