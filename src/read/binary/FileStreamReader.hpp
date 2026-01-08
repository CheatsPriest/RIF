#pragma once


#include <char.hpp>
#include <fstream>
#include <vector>
#include <unicode/ucsdet.h>
#include <unicode/ucnv.h>
#include <read/BaseReader.hpp>

class FileStreamReader : public BaseReader<FileStreamReader> {
private:
    std::unique_ptr<std::ifstream> file_ptr;
    UConverter* converter = nullptr;
    std::vector<char> inBuffer;
    string outBuffer;

public:
    FileStreamReader(const std::string& path) : inBuffer(CHUNK_SIZE) {
        file_ptr = std::make_unique< std::ifstream>();
        file_ptr->open(path, std::ios::binary);
        if (!file_ptr->is_open()) return;

        // 1. Детектируем кодировку
        UErrorCode status = U_ZERO_ERROR;
        file_ptr->read(inBuffer.data(), CHUNK_SIZE);
        int32_t bytesRead = static_cast<int32_t>(file_ptr->gcount());

        UCharsetDetector* csd = ucsdet_open(&status);
        ucsdet_setText(csd, inBuffer.data(), bytesRead, &status);
        const UCharsetMatch* match = ucsdet_detect(csd, &status);

        // Если файл пустой или не распознан, ставим UTF-8 по умолчанию
        const char* encoding = (match) ? ucsdet_getName(match, &status) : "UTF-8";

        // 2. Создаем конвертер
        converter = ucnv_open(encoding, &status);
        ucsdet_close(csd);

        // --- КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ ---
        file_ptr->clear(); // Сбрасываем флаги ошибок (включая EOF), если файл был меньше 64КБ
        file_ptr->seekg(0, std::ios::beg); // Теперь seekg сработает корректно
    }

    // Основная функция: читает следующий кусок и возвращает его в UTF-16
    bool readNextChunkImpl(string& chunk) {
        if (!file_ptr->is_open() || file_ptr->eof()) return false;

        file_ptr->read(inBuffer.data(), CHUNK_SIZE);
        int32_t bytesRead = static_cast<int32_t>(file_ptr->gcount());
        if (bytesRead <= 0) return false;

        UErrorCode status = U_ZERO_ERROR;

        // Резервируем место в выходной строке (UTF-16 не может быть длиннее байтов)
        chunk.resize(bytesRead);

        const char* source = inBuffer.data();
        const char* sourceLimit = source + bytesRead;
        UChar* target = reinterpret_cast<UChar*>(chunk.data());
        UChar* targetLimit = target + chunk.size();

        // Конвертируем. ICU сам сохранит "огрызки" символов в объекте converter
        ucnv_toUnicode(converter, &target, targetLimit,
            &source, sourceLimit, nullptr, file_ptr->eof(), &status);

        // Подрезаем строку под реальный размер
        chunk.resize(target - reinterpret_cast<UChar*>(chunk.data()));
        return true;
    }
    FileStreamReader(FileStreamReader&& other) noexcept
        : file_ptr(std::move(other.file_ptr)),
        converter(other.converter),
        inBuffer(std::move(other.inBuffer)),
        outBuffer(std::move(other.outBuffer))
    {
        other.converter = nullptr; // КРИТИЧНО: зануляем, чтобы деструктор не убил конвертер
    }

    // Также нужно перемещающее присваивание
    FileStreamReader& operator=(FileStreamReader&& other) noexcept {
        if (this != &other) {
            if (converter) ucnv_close(converter);
            file_ptr = std::move(other.file_ptr);
            converter = other.converter;
            inBuffer = std::move(other.inBuffer);
            outBuffer = std::move(other.outBuffer);
            other.converter = nullptr;
        }
        return *this;
    }
    ~FileStreamReader() {
        if (converter) ucnv_close(converter);
    }
};