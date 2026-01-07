#pragma once
#include <read/UnifiedReader.hpp>
#include <configs/SearchConfig.hpp>
#include <vector>
#include <search/SearchRawResult.hpp>
#include <iostream>

/*
ИНИЦИАЛИЗИРОВАТЬ Состояние = 0
ПОКА есть символы в файле:
    СИМВОЛ = ЧитатьСледующийСимвол()

    ЕСЛИ ТекущееУсловие == "ТЕКСТ":
        ЕСЛИ СИМВОЛ совпадает с ожидаемым в тексте:
            Продвинуть внутренний счетчик
            ЕСЛИ текст кончился: Перейти к следующему условию
        ИНАЧЕ: Сбросить счетчик текста

    ИНАЧЕ ЕСЛИ ТекущееУсловие == "ОДИН_ЛЮБОЙ":
        Перейти к следующему условию (пропустив этот СИМВОЛ)

    ИНАЧЕ ЕСЛИ ТекущееУсловие == "ЛЮБАЯ_ПОСЛЕДОВАТЕЛЬНОСТЬ":
        ЕСЛИ СИМВОЛ == СледующийМаркер:
            Перейти к следующему условию
        ИНАЧЕ:
            Продолжать (игнорировать символ)

    ЕСЛИ все условия выполнены:
        ВЫВЕСТИ "Найдено"
        СБРОСИТЬ Состояние = 0
*/


class SearchExact {
private:
    SearchConfig& config;
    Lowercaser lower_case;
public:
    SearchExact() : config(SearchConfig::get()) {

    }
    std::vector<RawResult> search(UnifiedReader& reader) {

        std::vector<RawResult> result;
        result.reserve(4);

        size_t i = 0;
        size_t len = config.exact_templ.size();
        const auto& templ = config.exact_templ;
        while (!reader.empty()) {
            const char_t cur = config.respect_registers? reader.readSymbol() : lower_case(reader.readSymbol());
            
            //std::cout << cur << std::endl;
            if (templ[i] == config.joker_symbol) {
                i++;
                reader.moveToSymbol(1);
            }
            else if (templ[i] == config.substring_symbol) {
                if (templ[0] == cur) {
                    i = 0;
                }
                else if (templ[i + 1] == cur) {
                    i+=2;
                    reader.moveToSymbol(1);
                }
                else {
                    reader.moveToSymbol(1);
                }
            }
            else {
                if (templ[i] == cur) {
                    i++;
                    reader.moveToSymbol(1);
                }
                else if(templ[i] != cur and i!=0){
                    i = 0;
                }
                else {
                    reader.moveToSymbol(1);
                }
            }
            if (i == len) {
                if (config.full_words) {
                    std::cout << "Find on: " << reader.getPos() - len << std::endl;
                }
                else {
                    std::cout << "Find on: " << reader.getPos() - len << std::endl;
                }
                result.push_back({ reader.getPos() - len, reader.getPos() });
                i = 0;
            }
        }


        return result;
    }
};

