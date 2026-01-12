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

    Lowercaser lowercaser;

    string chunk;
    bool isEmpty = false;

    bool is_lowercase=false;

    long long curLen=0;
    long long curMaxLen=0;
    size_t position=0;


    bool loadNextChunkFromBackend()  {
        string& current_chunk = chunk;
        return std::visit([&](auto& r) {
            return r.readNextChunkImpl(current_chunk);
            }, reader);
    }

    void loadNextChunk()  {
        curLen -= curMaxLen;
        isEmpty = !loadNextChunkFromBackend();
        if (is_lowercase) {
            lowercaser(chunk);
        }
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
        is_lowercase = !config.respect_registers;
    }

    explicit UnifiedReader(reader_v&& r)
        : reader(std::move(r)), config(SearchConfig::get() ), prev_word_size(0)
        , curLen(0), curMaxLen(0), position(0) {
        loadNextChunk();
        is_lowercase = !config.respect_registers;
    }
    void setIsLowercase(bool value) {
        is_lowercase = value;
    }
    inline char_t readSymbol()  {

        return chunk[curLen];

    }
    inline void moveToSymbol(long long dif)  {
        position += dif;
        curLen += dif;

        // Позволяем компилятору встроить "быстрый путь"
        if ((curLen >= curMaxLen)) {
            handleBufferOverflow();
        }
    }
    

    size_t getPos() const  {
        return position;
    }
    size_t getWordSize() const  {
        return prev_word_size;
    }
    
    bool empty()  {
        return isEmpty;
    }


    void rewind() {
        // Перемотка в начало - если нет специального метода,
        // закрываем и открываем заново или ищем способ сбросить позицию
        // Пока заглушка
    }

    ~UnifiedReader() {
        SearchStats::get().kbytes_read.fetch_add(position/1024*sizeof(char));
    }
private:
    

public:
    void skipSeparators()  {
        while (!empty() && is_separator(readSymbol())) {
            moveToSymbol(1);
        }
    }
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
    bool moveToNextWord()   {
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