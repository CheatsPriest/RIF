#pragma once
#include <read/ReadersVariants.hpp>

#include <string>
#include <string_view>
#include <functional>
#include <vector>
#include <cctype>
#include <algorithm>

#include <configs/SearchConfig.hpp>


reader_v openFile(const std::string& fileName);

class UnifiedReader {
private:
    reader_v reader;
    SearchConfig& config;
    size_t prev_word_size;

    string chunk;
    bool isEmpty = false;

    long long curLen=0;
    long long curMaxLen=0;
    size_t position=0;

    bool loadNextChunkFromBackend() noexcept {
        string& current_chunk = chunk;
        return std::visit([&](auto& r) {
            return r.readNextChunkImpl(current_chunk);
            }, reader);
    }

    void loadNextChunk() noexcept {
        curLen -= curMaxLen;
        isEmpty = !loadNextChunkFromBackend();
        curMaxLen = chunk.size();
    }
    void handleBufferOverflow() {
        while (curLen >= curMaxLen && !isEmpty) {
            loadNextChunk();
        }
    }
public:
    
    // Конструкторы
    explicit UnifiedReader(const std::string& filename)
        : reader(openFile(filename)), config(SearchConfig::get()), prev_word_size(0)
    , curLen(0), curMaxLen(0), position(0){
        loadNextChunk();
    }

    explicit UnifiedReader(reader_v&& r)
        : reader(std::move(r)), config(SearchConfig::get() ), prev_word_size(0)
        , curLen(0), curMaxLen(0), position(0) {
        loadNextChunk();
    }

    inline char_t readSymbol() noexcept {

        return chunk[curLen];

    }
    inline void moveToSymbol(long long dif) noexcept {
        position += dif;
        curLen += dif;

        // Позволяем компилятору встроить "быстрый путь"
        if ((curLen >= curMaxLen)) {
            handleBufferOverflow();
        }
    }
    

    size_t getPos() const noexcept {
        return position;
    }
    size_t getWordSize() const noexcept {
        return prev_word_size;
    }
    
    bool empty() noexcept {
        return isEmpty;
    }


    void rewind() {
        // Перемотка в начало - если нет специального метода,
        // закрываем и открываем заново или ищем способ сбросить позицию
        // Пока заглушка
    }

    // === РАБОТА СО СЛОВАМИ ===

private:
    // Пропустить разделители
    void skipSeparators() noexcept {
        while (!empty() && is_separator(readSymbol())) {
            moveToSymbol(1);
        }
    }

public:
    // Прочитать слово
    string readWord() {
        if (empty()) return {};

        skipSeparators();
        if (empty()) return {};

        string res;
        res.reserve(16);//средняя длина слов
        while (!empty() && !is_separator(readSymbol())) {
            res += readSymbol();
            moveToSymbol(1);
        }
        prev_word_size = res.size();
        return (std::move(res));
    }

    // Перейти к следующему слову
    bool moveToNextWord() noexcept  {
        skipSeparators();
        return !empty();
    }
    string loadContext(long long left_pos, long long right_pos) {
        long long left = (left_pos > config.left_context) ? left_pos - config.left_context : 0ll;
        
        string context;
        long long zone_size = config.right_context + right_pos - left ;//чтобы если left = 0, то не выделять лишнюю память
        context.reserve(zone_size); 

        moveToSymbol(left);
        for (long long i = 0; i < zone_size and !empty(); i++) {
            context += readSymbol();
            moveToSymbol(1);
        }
        
        return (std::move(context));
    }
};