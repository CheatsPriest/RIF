#pragma once


#include <char.hpp>
#include <fstream>
#include <vector>
#include <unicode/ucsdet.h>
#include <unicode/ucnv.h>

class FileStreamReader {
private:
    std::ifstream file;
    UConverter* converter = nullptr;
    std::vector<char> inBuffer;
    string outBuffer;

    static constexpr size_t CHUNK_SIZE = 65536; // 64КБ для оптимального кэша

public:
    FileStreamReader(const std::string& path) : inBuffer(CHUNK_SIZE) {
        file.open(path, std::ios::binary);
        if (!file.is_open()) return;

        // 1. Детектируем кодировку
        UErrorCode status = U_ZERO_ERROR;
        file.read(inBuffer.data(), CHUNK_SIZE);
        int32_t bytesRead = static_cast<int32_t>(file.gcount());

        UCharsetDetector* csd = ucsdet_open(&status);
        ucsdet_setText(csd, inBuffer.data(), bytesRead, &status);
        const UCharsetMatch* match = ucsdet_detect(csd, &status);

        // Если файл пустой или не распознан, ставим UTF-8 по умолчанию
        const char* encoding = (match) ? ucsdet_getName(match, &status) : "UTF-8";

        // 2. Создаем конвертер
        converter = ucnv_open(encoding, &status);
        ucsdet_close(csd);

        // --- КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ ---
        file.clear(); // Сбрасываем флаги ошибок (включая EOF), если файл был меньше 64КБ
        file.seekg(0, std::ios::beg); // Теперь seekg сработает корректно
    }

    // Основная функция: читает следующий кусок и возвращает его в UTF-16
    bool readNextChunk(string& chunk) {
        if (!file.is_open() || file.eof()) return false;

        file.read(inBuffer.data(), CHUNK_SIZE);
        int32_t bytesRead = static_cast<int32_t>(file.gcount());
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
            &source, sourceLimit, nullptr, file.eof(), &status);

        // Подрезаем строку под реальный размер
        chunk.resize(target - reinterpret_cast<UChar*>(chunk.data()));
        return true;
    }

    ~FileStreamReader() {
        if (converter) ucnv_close(converter);
    }
};