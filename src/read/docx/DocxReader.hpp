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

    std::string filename; // Путь к файлу остается в UTF-8
    std::generator<std::string> gen_obj; // DuckX отдает UTF-8
    using gen_iterator = decltype(gen_obj.begin());
    gen_iterator curIter;

    // Вспомогательный метод для конвертации UTF-8 -> UTF-16
    static string to_utf16(const std::string& utf8_str) {
        return icu::UnicodeString::fromUTF8(utf8_str).getTerminatedBuffer();
    }
    bool has_text;
    std::generator<std::string> create_walker() {
        duckx::Document doc(filename);
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
        if (!buffer.empty()) {
            co_yield std::move(buffer);
        }
        has_text = false;
    }

    

public:
    DocxReader(const std::string& filename)
        : filename(filename),
        gen_obj(create_walker()),
        curIter(gen_obj.begin())
    {
        has_text = (curIter != gen_obj.end());
    }
    bool readNextChunkImpl(string& chunk) {

        if (curIter == gen_obj.end() or !has_text) return false;
        chunk.clear();
        chunk = to_utf16(*curIter);

        ++curIter;

        return true;
    }
    
};