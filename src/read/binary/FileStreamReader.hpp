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
    bool is_native_utf8 = false;

public:
    FileStreamReader(const std::string& path) : inBuffer(CHUNK_SIZE) {
        file_ptr = std::make_unique<std::ifstream>(path, std::ios::binary);
        if (!file_ptr->is_open()) return;

        UErrorCode status = U_ZERO_ERROR;
        file_ptr->read(inBuffer.data(), CHUNK_SIZE);
        int32_t bytesRead = static_cast<int32_t>(file_ptr->gcount());

        // Для очень маленьких файлов или пустых
        if (bytesRead <= 0) {
            is_native_utf8 = true;  // Пустой файл = UTF-8 по умолчанию
            resetFilePosition();
            return;
        }

        UCharsetDetector* csd = ucsdet_open(&status);
        ucsdet_setText(csd, inBuffer.data(), bytesRead, &status);
        const UCharsetMatch* match = ucsdet_detect(csd, &status);

        const char* encoding = (match) ? ucsdet_getName(match, &status) : "UTF-8";

        // Проверяем все возможные варианты UTF-8 имен
        std::string encoding_str(encoding);
        if (encoding_str == "UTF-8" || encoding_str == "UTF8" ||
            encoding_str == "US-ASCII" || encoding_str.find("UTF-8") != std::string::npos ||
            encoding_str.find("UTF8") != std::string::npos) {
            is_native_utf8 = true;
        }
        else {
            converter = ucnv_open(encoding, &status);
            if (U_FAILURE(status)) {
                // Если не смогли открыть конвертер, fallback на UTF-8
                is_native_utf8 = true;
                converter = nullptr;
            }
        }

        ucsdet_close(csd);
        resetFilePosition();
    }

    bool readNextChunkImpl(string& chunk) {
        if (!file_ptr->is_open() || file_ptr->eof()) {
            return false;
        }

        // ПУТЬ 1: Файл уже в UTF-8 (быстрый путь)
        if (is_native_utf8) {
            return readUTF8Chunk(chunk);
        }

        // ПУТЬ 2: Конвертация из другой кодировки в UTF-8
        if (converter) {
            return readAndConvertChunk(chunk);
        }

        // Запасной путь (не должно случиться)
        return readUTF8Chunk(chunk);
    }

private:
    void resetFilePosition() {
        if (file_ptr->is_open()) {
            file_ptr->clear();
            file_ptr->seekg(0, std::ios::beg);
        }
    }

    // Чтение UTF-8 файла (без конвертации)
    bool readUTF8Chunk(string& chunk) {
        chunk.resize(CHUNK_SIZE);
        file_ptr->read((char *)chunk.data(), CHUNK_SIZE);
        size_t bytesRead = file_ptr->gcount();

        if (bytesRead <= 0) {
            return false;
        }

        chunk.resize(bytesRead);
        return true;
    }

    // Чтение и конвертация в UTF-8
    bool readAndConvertChunk(string& chunk) {
        // 1. Читаем сырые байты из файла
        file_ptr->read(inBuffer.data(), CHUNK_SIZE);
        int32_t bytesRead = static_cast<int32_t>(file_ptr->gcount());

        if (bytesRead <= 0) {
            return false;
        }

        UErrorCode status = U_ZERO_ERROR;

        // 2. Получаем максимальный возможный размер UTF-8 вывода
        //    В худшем случае (например, UTF-32) каждый символ может стать 4 байтами UTF-8
        int32_t maxOutputSize = bytesRead * 4;

        // 3. Подготавливаем буфер для вывода
        chunk.resize(maxOutputSize);

        // 4. Конвертируем
        const char* source = inBuffer.data();
        const char* sourceLimit = source + bytesRead;
        char* target = (char*)chunk.data();
        const char* targetLimit = target + maxOutputSize;

        // ucnv_toAlgorithmic правильно конвертирует в UTF-8
        ucnv_toAlgorithmic(UCNV_UTF8, converter,
            target, maxOutputSize,
            source, bytesRead, &status);

        // 5. Проверяем результат
        if (U_FAILURE(status) && status != U_BUFFER_OVERFLOW_ERROR) {
            // Ошибка конвертации
            chunk.clear();
            return false;
        }

        // 6. Устанавливаем реальный размер
        chunk.resize(target - (char *)chunk.data());

        return chunk.size() > 0;
    }

public:
    FileStreamReader(FileStreamReader&& other) noexcept
        : file_ptr(std::move(other.file_ptr)),
        converter(other.converter),
        inBuffer(std::move(other.inBuffer)),
        is_native_utf8(other.is_native_utf8)
    {
        other.converter = nullptr;
        other.is_native_utf8 = true;
    }

    FileStreamReader& operator=(FileStreamReader&& other) noexcept {
        if (this != &other) {
            if (converter) {
                ucnv_close(converter);
            }
            file_ptr = std::move(other.file_ptr);
            converter = other.converter;
            inBuffer = std::move(other.inBuffer);
            is_native_utf8 = other.is_native_utf8;
            other.converter = nullptr;
            other.is_native_utf8 = true;
        }
        return *this;
    }

    ~FileStreamReader() {
        if (converter) {
            ucnv_close(converter);
        }
    }

    // Вспомогательные методы
    bool isUTF8() const { return is_native_utf8; }
    bool needsConversion() const { return converter != nullptr; }
};


/*
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
    bool is_native_utf8 = false; // Флаг для пропуска конвертации

public:
    FileStreamReader(const std::string& path) : inBuffer(CHUNK_SIZE) {
        file_ptr = std::make_unique<std::ifstream>(path, std::ios::binary);
        if (!file_ptr->is_open()) return;

        UErrorCode status = U_ZERO_ERROR;
        file_ptr->read(inBuffer.data(), CHUNK_SIZE);
        int32_t bytesRead = static_cast<int32_t>(file_ptr->gcount());

        UCharsetDetector* csd = ucsdet_open(&status);
        ucsdet_setText(csd, inBuffer.data(), bytesRead, &status);
        const UCharsetMatch* match = ucsdet_detect(csd, &status);

        const char* encoding = (match) ? ucsdet_getName(match, &status) : "UTF-8";

        // Если файл уже в UTF-8 или ASCII, нам не нужен тяжелый ICU
        if (std::string(encoding) == "UTF-8" || std::string(encoding) == "US-ASCII") {
            is_native_utf8 = true;
        }
        else {
            converter = ucnv_open(encoding, &status);
        }

        ucsdet_close(csd);
        file_ptr->clear();
        file_ptr->seekg(0, std::ios::beg);
    }

    bool readNextChunkImpl(string& chunk) {
        if (!file_ptr->is_open() || file_ptr->eof()) return false;

        // ПУТЬ 1: Скоростной (Zero-overhead UTF-8)
        if (is_native_utf8) {
            chunk.resize(CHUNK_SIZE);
            file_ptr->read(reinterpret_cast<char*>(chunk.data()), CHUNK_SIZE);
            size_t bytes = file_ptr->gcount();
            if (bytes <= 0) return false;
            chunk.resize(bytes);
            return true;
        }

        // ПУТЬ 2: Конвертация (Legacy/Other encodings -> UTF-8)
        static std::vector<char> utf8_buffer(CHUNK_SIZE * 2); // Буфер для ICU
        file_ptr->read(inBuffer.data(), CHUNK_SIZE);
        int32_t bytesRead = static_cast<int32_t>(file_ptr->gcount());
        if (bytesRead <= 0) return false;

        UErrorCode status = U_ZERO_ERROR;

        const char* source = inBuffer.data();
        int32_t  sourceLimit =  bytesRead;

        chunk.resize(CHUNK_SIZE * 2); // UTF-8 может быть шире оригинала
        char* target = reinterpret_cast<char*>(chunk.data());
        int32_t targetLimit = chunk.size();

        ucnv_toAlgorithmic(UCNV_UTF8, converter, target, targetLimit,
            source, sourceLimit, &status);

        chunk.resize(target - reinterpret_cast<char*>(chunk.data()));
        return true;
    }
    FileStreamReader(FileStreamReader&& other) noexcept
        : file_ptr(std::move(other.file_ptr)),
        converter(other.converter),
        inBuffer(std::move(other.inBuffer))
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
            other.converter = nullptr;
        }
        return *this;
    }
    ~FileStreamReader() {
        if (converter) ucnv_close(converter);
    }
};*/