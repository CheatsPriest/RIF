#pragma once
#include <read/BaseReader.hpp>
#include "duckx.hpp"
#include <generator>
#include <char.hpp>
#include <iostream>

#include <unicode/unistr.h>



#include <unicode/unistr.h>
#include <generator>
#include <string>

class DocxReader : public BaseReader<DocxReader> {
private:
    std::string filename;
    std::generator<std::string> gen_obj;
    using gen_iterator = decltype(gen_obj.begin());
    static std::u16string to_utf16(const std::string& utf8_str) {
        icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(utf8_str);
        return std::u16string(reinterpret_cast<const char16_t*>(ustr.getBuffer()), ustr.length());
    }

    std::generator<std::string> create_walker(std::string path) {
        duckx::Document doc(path);
        doc.open();
        if (!doc.is_open()) {
            co_return;
        }

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
    }

    std::unique_ptr<gen_iterator> it;
    bool initialized = false;

public:
    DocxReader(const std::string& filename)
        : filename(filename),
        gen_obj(create_walker(filename)), it(nullptr)
    {
       
    }

    DocxReader(const DocxReader&) = delete;
    DocxReader(DocxReader&&) = default;

    bool readNextChunkImpl(std::u16string& chunk) {
        if (!initialized) {
            it = std::make_unique< gen_iterator>(gen_obj.begin());
            initialized = true;
        }

        if (*it == gen_obj.end()) {
            return false;
        }

        chunk = to_utf16(**it);

        ++*it;

        return true;
    }
    void rewindImpl() {
        it.reset();
        gen_obj = create_walker(filename);
        it = std::make_unique<gen_iterator>(gen_obj.begin());
        initialized = true;
    }
};
