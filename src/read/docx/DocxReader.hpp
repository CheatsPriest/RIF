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
    // Оставляем генератор, но убираем хранение итератора как поля класса
    std::generator<std::string> gen_obj;
    using gen_iterator = decltype(gen_obj.begin());
    // Используем std::u16string (или ваш string_t / char_t)
    // Важно: ICU работает с UChar (обычно это char16_t)
    static std::u16string to_utf16(const std::string& utf8_str) {
        icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(utf8_str);
        return std::u16string(reinterpret_cast<const char16_t*>(ustr.getBuffer()), ustr.length());
    }

    // Параметризуем создание воркера, чтобы не зависеть от состояния полей во время инициализации
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

    // Храним текущее состояние итерации через сам итератор генератора
    // Но инициализируем его только один раз
    std::unique_ptr<gen_iterator> it;
    bool initialized = false;

public:
    DocxReader(const std::string& filename)
        : filename(filename),
        gen_obj(create_walker(filename)), it(nullptr)
    {
       
        // Не вызываем begin() здесь, если есть риск перемещения объекта
    }

    // В C++23 генераторы move-only, запретим копирование явно
    DocxReader(const DocxReader&) = delete;
    DocxReader(DocxReader&&) = default;

    bool readNextChunkImpl(std::u16string& chunk) {
        // Ленивая инициализация итератора
        if (!initialized) {
            it = std::make_unique< gen_iterator>(gen_obj.begin());
            initialized = true;
        }

        if (*it == gen_obj.end()) {
            return false;
        }

        // 1. Конвертируем текущую строку из UTF-8 в UTF-16
        // 2. Кладем результат прямо в chunk
        chunk = to_utf16(**it);

        // Продвигаем итератор к следующему co_yield
        ++*it;

        return true;
    }
};
