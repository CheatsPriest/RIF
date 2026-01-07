#pragma once
#include <read/BaseReader.hpp>
#include "duckx.hpp"
#include <generator>
#include <char.hpp>
#include <iostream>

#include <unicode/unistr.h>

class DocxReader : public BaseReader<DocxReader> {
private:
    std::string filename; // Путь к файлу остается в UTF-8
    std::generator<std::string> gen_obj; // DuckX отдает UTF-8
    using gen_iterator = decltype(gen_obj.begin());
    gen_iterator curIter;

    // Вспомогательный метод для конвертации UTF-8 -> UTF-16
    static string to_utf16(const std::string& utf8_str) {
        return icu::UnicodeString::fromUTF8(utf8_str).getTerminatedBuffer();
    }

    std::generator<std::string> create_walker() {
        duckx::Document doc(filename);
        doc.open();
        if (!doc.is_open()) co_return; // DuckX может кинуть исключение или вернуть false

        for (auto p : doc.paragraphs()) {
            for (auto r : p.runs()) {
                co_yield (r.get_text());
            }
        }
    }

    string curText; // Теперь это std::u16string
    long long curLen;
    long long curMaxLen;
    size_t position;

public:
    DocxReader(const std::string& filename)
        : filename(filename),
        gen_obj(create_walker()),
        curIter(gen_obj.begin()),
        curLen(0),
        curMaxLen(0),
        position(0)
    {
        if (curIter != gen_obj.end()) {
            // Конвертируем первый кусок текста в UTF-16
            curText = to_utf16(*curIter);
            curText += u' '; // Используем префикс u для char16_t
            curMaxLen = curText.size();
        }
    }

    // Возвращает char16_t
    const char_t readSymbolImpl() const {
        if (curLen < curMaxLen) {
            return curText[curLen];
        }
        return u'\0';
    }

    bool moveToSymbolImpl(long long dif) {
        position += dif;
        curLen += dif;

        while (curLen >= curMaxLen) {
            ++curIter;
            if (curIter != gen_obj.end()) {
                curLen -= curMaxLen;
                curText = to_utf16(*curIter);
                curText += u' ';
                curMaxLen = curText.size();

            }
            else {
                curMaxLen = 0;
                curLen = 0;
                return false;
            }
        }
        return true;
    }

    bool emptyImpl() const {
        return curIter == gen_obj.end();
    }

    void restartImpl() {
        gen_obj = create_walker();
        curIter = gen_obj.begin();
        position = 0;
    }

    size_t getPosImpl() const {
        return position;
    }
};