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
public:
    char_t readSymbol() const {
        return std::visit([](const auto& r) { return r.readSymbol(); }, reader);
    }
    // Конструкторы
    explicit UnifiedReader(const std::string& filename)
        : reader(openFile(filename)), config(SearchConfig::get()), prev_word_size(0){
    }

    explicit UnifiedReader(reader_v&& r)
        : reader(std::move(r)), config(SearchConfig::get() ), prev_word_size(0){
    }
    

    
    /*const char_t* getData() {
        return std::visit([](const auto& r) { return r.getData(); }, reader);
    }*/

    bool moveToSymbol(long long diff) {
        return std::visit([diff](auto& r) { return r.moveToSymbol(diff); }, reader);
    }

    size_t getPos() const {
        return std::visit([](const auto& r) { return r.getPos(); }, reader)- prev_word_size;
    }
    
    bool empty()  {
        return std::visit([]( auto& r) { return r.empty(); }, reader);
    }

    const char_t* getPtr() const {
        return std::visit([](const auto& r) {
            // У BaseReader нет getPtr, но у наследников может быть
            // Если нужен - добавим в BaseReader
            return static_cast<const char_t*>(nullptr);
            }, reader);
    }


    void rewind() {
        // Перемотка в начало - если нет специального метода,
        // закрываем и открываем заново или ищем способ сбросить позицию
        // Пока заглушка
    }

    // === РАБОТА СО СЛОВАМИ ===

private:
    // Пропустить разделители
    void skipSeparators() {
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
        return formatToLocal(std::move(res));
    }

    // Перейти к следующему слову
    bool moveToNextWord() {
        skipSeparators();
        return !empty();
    }
    string loadContext(long long pos) {
        long long left = (pos > config.left_context) ? pos - config.left_context : 0ll;
        
        string context;
        long long zone_size = config.right_context + pos - left + config.raw_templ.size();//чтобы если left = 0, то не выделять лишнюю память
        context.reserve(zone_size); 

        moveToSymbol(left);
        for (long long i = 0; i < zone_size and !empty(); i++) {
            context += readSymbol();
            moveToSymbol(1);
        }
        
        return formatToLocal(std::move(context));
    }
};