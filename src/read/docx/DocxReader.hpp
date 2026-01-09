#pragma once
#include <read/BaseReader.hpp>
#include "duckx.hpp"
#include <generator>
#include <char.hpp>
#include <iostream>

#include <unicode/unistr.h>



class DocxReader : public BaseReader<DocxReader> {
private:
    static constexpr char_t next_line_symbol = u'\n';
    duckx::Document doc;

    std::string filename; // Путь к файлу остается в UTF-8
    std::generator<std::string> gen_obj; // DuckX отдает UTF-8
    using gen_iterator = decltype(gen_obj.begin());
    gen_iterator curIter;

    // Вспомогательный метод для конвертации UTF-8 -> UTF-16
    static std::u16string to_utf16(const std::string& utf8_str) {
        icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(utf8_str);
        // Копируем данные в стандартную строку UTF-16
        return std::u16string(ustr.getBuffer(), ustr.length());
    }

    bool has_text;
    std::generator<std::string> create_walker() {
        
        doc.open();
        if (!doc.is_open()) co_return;
        std::string buffer;
        buffer.reserve(CHUNK_SIZE + 1024);

        for (auto p : doc.paragraphs()) {
            for (auto r : p.runs()) {
                buffer += r.get_text();
            }
            buffer += '\n';
            if (buffer.size() >= CHUNK_SIZE) {
                co_yield std::move(buffer);
                buffer.clear();
                buffer.reserve(CHUNK_SIZE + 1024);
            }
        }

        has_text = false;
        if (!buffer.empty()) {
            co_yield std::move(buffer);
        }
        while (true) {
            co_yield "";
        }
    }

    

public:
    DocxReader(const std::string& filename)
        : filename(filename), doc(filename),
        gen_obj(create_walker()),
        curIter(gen_obj.begin())
    {
        has_text = (curIter != gen_obj.end());
    }
    bool readNextChunkImpl(string& chunk) {

        std::cout << "Enternered read" << std::endl;
        chunk.clear();
        if (curIter == gen_obj.end() or !has_text) return false;
        std::cout << "Not empty" << std::endl;
        chunk = to_utf16(*curIter);

        ++curIter;
        std::cout << "Chunk size "<< chunk.size() << std::endl;
        if (chunk.size() == 0) {
            return false;
        }
        return has_text;
    }
    
};