#pragma once
#include <read/ReadersVariants.hpp>

#include <string>
#include <string_view>
#include <functional>
#include <vector>
#include <cctype>
#include <algorithm>

#include <configs/SearchConfig.hpp>
#include <language_deductor/LanguageDeductor.hpp>


reader_v openFile(const std::filesystem::path& fileName);

class UnifiedReader {
private:
    reader_v reader;
    SearchConfig& config;

    Lowercaser lowercaser;

    string chunk;
    bool isEmpty = false;

    bool is_lowercase=true;

    long long curLen=0;
    long long curMaxLen=0;
    size_t position=0;
    string local_word;

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
    explicit UnifiedReader(const std::filesystem::path& filename, bool is_lowercased)
        : reader(openFile(filename)), config(SearchConfig::get())
    , curLen(0), curMaxLen(0), position(0), is_lowercase(is_lowercased){
        loadNextChunk();
        local_word.reserve(128);
    }

    explicit UnifiedReader(reader_v&& r, bool is_lowercased)
        : reader(std::move(r)), config(SearchConfig::get())
        , curLen(0), curMaxLen(0), position(0), is_lowercase(is_lowercased) {
        loadNextChunk();
        local_word.reserve(128);
    }
    void setIsLowercase(bool value) {
        is_lowercase = value;
    }
    inline char_t readSymbol() noexcept{
        return chunk[curLen];
    }
    inline void moveToSymbol(long long dif) noexcept {
        position += dif;
        curLen += dif;

        if ((curLen >= curMaxLen)) {
            handleBufferOverflow();
        }
    }
    

    size_t getPos() const  {
        return position;
    }
    bool empty()  {
        return isEmpty;
    }
    void rewind() {
        
    }
    ~UnifiedReader() {
        SearchStats::get().kbytes_read.fetch_add(position/1024*sizeof(char));
    }
private:
    

public:
    void skipSeparators()  {
        while (!empty() and !isKnownSymbol(readSymbol())) {
            moveToSymbol(1);
        }
    }
    void skipSeparatorsOld() {
        while (!empty() and is_separator(readSymbol())) {
            moveToSymbol(1);
        }
    }
    // Прочитать слово
    string_view readWord() {
        if (empty()) return {};

        skipSeparators();
        if (empty()) return {};

        local_word.clear();
        while (!empty() and isKnownSymbol(readSymbol())) {
            local_word += readSymbol();
            moveToSymbol(1);
        }
        
        return local_word;
    }

    // Перейти к следующему слову
    bool moveToNextWord()   {
        skipSeparators();
        return !empty();
    }

    string_view readWordOld() {
        if (empty()) return {};

        skipSeparatorsOld();
        if (empty()) return {};

        local_word.clear();
        while (!empty() and !is_separator(readSymbol())) {
            local_word += readSymbol();
            moveToSymbol(1);
        }
        return local_word;
    }

    // Перейти к следующему слову
    bool moveToNextWordOld() {
        skipSeparatorsOld();
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
        
        return context;
    }
    
    
    inline string_view getChunk() const noexcept {
        return string_view(chunk.begin()+ curLen, chunk.end());
    }
    inline void refreshChunk() noexcept {
        moveToSymbol(curMaxLen);
    }

};