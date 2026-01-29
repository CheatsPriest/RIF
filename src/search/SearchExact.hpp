#pragma once
#include <read/UnifiedReader.hpp>
#include <configs/SearchConfig.hpp>
#include <vector>
#include <global/GlobalQueues.hpp>
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
    char_t joker_symbol;
    char_t substr_symbol;
    Lowercaser lower_case;

    string_view cur_chunk;


public:
    SearchExact() : config(SearchConfig::get()), joker_symbol(config.joker_symbol), substr_symbol(config.substring_symbol){

    }
    std::vector<ConcretePlace> search(UnifiedReader& reader) {

        std::vector<ConcretePlace> result;
        result.reserve(4);

        size_t len = config.exact_templ.size();
        const auto& templ = config.exact_templ;
        string_view view;
        

        while (!reader.empty()) {
            
            view = reader.getChunk();
            size_t pos = view.find(templ.front());
            

            if (pos != string_view::npos) {
                reader.moveToSymbol(pos);

                string_view stream = reader.getChunk();
                size_t i = 0;
                size_t pos_stream = 0;
                
                while (!reader.empty()) {
                    char_t cur;
                    if (pos_stream < stream.size()) {
                        cur = stream[pos_stream];
                    }
                    else {
                        stream = reader.getChunk();
                        pos_stream = 0;
                        continue;
                    }


                    //std::cout << cur << std::endl;
                    if (templ[i] == joker_symbol) {
                        i++;
                        pos_stream++;
                        reader.moveToSymbol(1);
                    }
                    //else if (templ[i] == substr_symbol) {
                    //    if (templ[0] == cur) {
                    //        //выход
                    //        break;
                    //    }
                    //    else if (templ[i + 1] == cur) {
                    //        i += 2;
                    //        reader.moveToSymbol(1);
                    //    }
                    //    else {
                    //        reader.moveToSymbol(1);
                    //    }
                    //}
                    else {
                        if (templ[i] == cur) {
                            i++;
                            pos_stream++;
                        }
                        else {
                            reader.moveToSymbol(i+1);
                            view = reader.getChunk();
                            //Выход
                            break;
                        }
                    }
                    if (i == len) {
                        reader.moveToSymbol(len);
                        result.push_back({ reader.getPos() - len, reader.getPos() });
                        view = reader.getChunk();
                        break;
                    }
                }
            }
            else {
                reader.refreshChunk();
                if(!reader.empty())view = reader.getChunk();
            }
        }

        


        return result;
    }
};

