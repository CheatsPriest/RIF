#pragma once
#include <read/BaseReader.hpp>
#include "duckx.hpp"
#include <generator>
#include <char.hpp>
#include <iostream>

#ifdef _WIN32

std::string utf8_to_cp1251(std::string&& utf8);

#endif

string format(string&& in);

class DocxReader : public BaseReader<DocxReader> {
private:
    std::string filename;
    std::generator<std::string> gen_obj;
    using gen_iterator = decltype(gen_obj.begin());

    gen_iterator curIter;

    

    std::generator<std::string> create_walker() {
        duckx::Document doc(filename);

        doc.open();
        if (!doc.is_open())co_return;
        for (auto p : doc.paragraphs()) {
            for (auto r : p.runs()) {
                co_yield format(r.get_text());
            }
        }
    }
    string curText;
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
            curText = *curIter;
            curText+=' ';
            curMaxLen = curText.size();
            
        }
        else {
            
        }
        std::cout << curMaxLen << std::endl;
    }


    const char_t readSymbolImpl()  const  {
        return curText[curLen];
    }
    bool moveToSymbolImpl(long long dif) {
        position += dif;
        curLen += dif;
        while (curLen >= curMaxLen and !emptyImpl()) {
            curLen -= curMaxLen;
            ++curIter;

            if (curIter != gen_obj.end()) {
                curText =  *curIter;
                curText += ' ';
                curMaxLen = curText.size();
            }
            else {
                curMaxLen = 0;
                curLen = 0;
                return false;
            }
        }
        return !emptyImpl();
    }
    bool emptyImpl() const {
        return curIter == gen_obj.end();
    }
    void restartImpl() {
        gen_obj = create_walker();
        curIter = gen_obj.begin();
    }
    size_t getPosImpl() const {
        return position;
    }
};